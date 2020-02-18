/*
@file EventScript.cpp
*/
#define _CRT_SECURE_NO_WARNINGS
#include "EventScript.h"
#include "GLFWEW.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <locale.h>
#include <stdlib.h>

/*
スクリプト命令の引数に値を設定する

@param arag スクリプト命令の引数
@param str 設定する値を含む文字列
*/
void EventScriptEngine::Set(EventScriptEngine::Argument& arg, const char* str)
{
	if (str[0] == '[') {
		VariableId id;
		if (sscanf(str, "[%d]", &id) >= 1) {
			arg = id;
		}
	}
	else {
		Number n;
		if (sscanf(str, "%lf", &n) >= 1) {
			arg = n;
		}
	}
}

/*
スクリプト命令の引数に関係演算子を設定する

@param arg スクリプト命令の引数
@param str 設定する値を含む文字列
*/
void EventScriptEngine::SetOperator(Argument& arg, const char* str)
{
	if (str[1] == '\0') {
		switch (str[0]) {
		case'<':arg = Operator::less; break;
		case'>':arg = Operator::greater; break;
		case'+':arg = Operator::add; break;
		case'-':arg = Operator::sub; break;
		case'*':arg = Operator::mul; break;
		case'/':arg = Operator::div; break;
		}
	}
	else if (str[1] == '=' && str[2] == '\0') {
		switch (str[0]) {
		case'=':arg = Operator::equal; break;
		case'!':arg = Operator::notEqual; break;
		case'<':arg = Operator::lessEqual; break;
		case'>':arg = Operator::greaterEqual; break;
		}
	}
}

/*
スクリプト命令の引数から値を取得する

@param arg スクリプト命令の引数

@return 引数から直接・間接に得られた値
*/
EventScriptEngine::Number EventScriptEngine::Get(
	const EventScriptEngine::Argument& arg) const
{
	if (const auto p = std::get_if<VariableId>(&arg)) {
		return variables[*p];
	}
	else if (const auto p = std::get_if<Number>(&arg)) {
		return *p;
	}
	return 0;
}


/*
スクリプトエンジンのシングルトン・インスタンスを取得する

@return スクリプトエンジンのシングルトン・インスタンス
*/
EventScriptEngine& EventScriptEngine::Instance()
{
	static EventScriptEngine instance;
	return instance;
}

/*
スクリプトエンジンを初期化する

@retval true  初期化成功
@retval false 初期化失敗
*/
bool EventScriptEngine::Init()
{
	if (isInitialized) {
		std::cerr << "[エラー]EventScriptEngineは既に初期化されています\n";
		return false;
	}

	filename.reserve(256);
	script.reserve(2048);
	if (!textWindow.Init("Res/TextWindow.tga",
		glm::vec2(0, -248), glm::vec2(48, 32), glm::vec2(0))) {
		std::cerr << "[エラー]" << __func__ << ":スクリプトエンジンの初期化に失敗\n";
		return false;
	}
	variables.resize(100, 0.0);
	isInitialized = true;
	return true;
}

/*
イベント・スクリプトを実行する

@param filename スクリプト・ファイル名

@retval true  実行に成功
@retval false 実行に失敗
*/
bool EventScriptEngine::RunScript(const char*filename)
{
	if (!isInitialized) {
		return false;
	}

	//スクリプトファイルを読み込み、テキストウィンドウ表示のためにwchar_t型に変換する
	std::ifstream ifs(filename);
	if (!ifs) {
		std::cerr << "[エラー]" << __func__ << ":スクリプトファイル" << filename <<
			"を読み込めません\n";
		return false;
	}
	setlocale(LC_CTYPE, "ja-JP");

	script.clear();//以前のスクリプトを消去
	size_t lineCount = 0;
	std::string line;
	char buf[1000];
	char a[20], b[20],c[20],op[20];//スクリプト引数用
	std::vector<size_t>jumpStack;
	while (std::getline(ifs, line)) {
		//先頭の余白を除去する
		line.erase(0, line.find_first_not_of("\t\n"));
		++lineCount;

		Instruction inst;
		//print命令を読み込む
		int n = sscanf(line.c_str(), "print %999[^\n]", buf);
		if (n >= 1) {
			const size_t size = mbstowcs(nullptr, buf, 0);
			std::wstring text(size, L'\0');
			mbstowcs(&text[0], buf, size);
			inst.type = InstructionType::print;
			inst.arguments[0] = text;
			script.push_back(inst);
			continue;
		}

		//四則演算命令を読み取る
		n = sscanf(line.c_str(), "[%19[^]]] = %19[^-=!<>+*/] %19[-=!<>+*/] %19[^-=!<>+*/]",
			a, b, op, c);
		if (n >= 4) {
			inst.type = InstructionType::expression;
			inst.arguments[0] = static_cast<VariableId>(atoi(a));
			Set(inst.arguments[1], b);
			SetOperator(inst.arguments[2], op);
			Set(inst.arguments[3], c);
			script.push_back(inst);
			continue;
		}

		//代入命令を読み取る
		n = sscanf(line.c_str(), "[%19[^]]]=%19s", a, b);
		if (n >= 2) {
			inst.type = InstructionType::assign;
			inst.arguments[0] = static_cast<VariableId>(atoi(a));
			Set(inst.arguments[1], b);
			script.push_back(inst);
			continue;
		}

		//if命令を読み取る
		n = sscanf(line.c_str(), "if %19s %19s %19s", a, op, b);
		if (n >= 3) {
			inst.type = InstructionType::beginif;
			Set(inst.arguments[0], a);
			SetOperator(inst.arguments[1], op);
			Set(inst.arguments[2], b);
			script.push_back(inst);

			//ジャンプ先を設定できるようにif命令の位置を記録
			jumpStack.push_back(script.size() - 1);
			continue;
		}

		//endif命令を読み取る
		if (strncmp(line.c_str(), "endif", 5) == 0) {
			if (jumpStack.empty()) {
				std::cerr << "[エラー]" << __func__ << "endifが多すぎます(" <<
				lineCount << "行目)\n";
				continue;
			}
			const size_t p = jumpStack.back();
			jumpStack.pop_back();
			script[p].jump = script.size();
			continue;
		}
	}

	programCounter = 0;

	//パラメータを設定
	isFinished = false;
	this->filename = filename;

	std::cout << "[INFO]" << __func__ << ":スクリプトファイル" << filename << "を実行\n";
	return true;
}

/*
スクリプトエンジンの状態を更新する

@param deltaTime 前回の更新からの経過時間(秒)
*/
void EventScriptEngine::Update(float deltaTime)
{
	if (!isInitialized) {
		return;
	}

	//スクリプト未設定、または実行終了なら何もしない
	if (script.empty() || isFinished) {
		return;

	}
	//命令を実行する
	//実行を中断する場合はyield変数にtrueを設定
	for (bool yield = false; !yield; ) {
		//実行位置(programCounter)がスクリプトの命令数以上なら実行完了
		if (programCounter >= script.size()) {
			isFinished = true;
			break;
		}

		const auto& inst = script[programCounter];
		switch (inst.type) {
		case InstructionType::print:
			if (!textWindow.IsOpen()) {
				//ウィンドウが閉じているので文章を設定
				const auto p = std::get_if<Text>(&inst.arguments[0]);
				if (!p) {
					std::cerr << "[エラー]" << __func__ <<
						"print命令の引数はText型でなくてはなりません\n";
					++programCounter;
					break;
				}
				textWindow.Open(p->c_str());
			}
			else {
				//ウィンドウが開いてたら表示終了を待つ. 表示が終了したら、キー入力を待つ
				//キー入力があったら、ウィンドウを閉じて、次の命令の処理へ進む
				if (textWindow.IsFinished()) {
					const GamePad gamepad = GLFWEW::Window::Instance().GetGamePad();
					if (gamepad.buttonDown & (GamePad::A | GamePad::B | GamePad::START)) {
						textWindow.Close();
						++programCounter;
						continue;
					}
				}
			}
			yield = true;
			break;

		case InstructionType::expression:
			if (const auto a = std::get_if<VariableId>(&inst.arguments[0])) {
				if (const auto op = std::get_if<Operator>(&inst.arguments[2])) {
					const Number b = Get(inst.arguments[1]);
					const Number c = Get(inst.arguments[3]);
					switch (*op) {
					case Operator::add: variables[*a] = b + c; break;
					case Operator::sub: variables[*a] = b - c; break;
					case Operator::mul: variables[*a] = b * c; break;
					case Operator::div: variables[*a] = b / c; break;
					}
				}
			}
			++programCounter;
			break;

		case InstructionType::assign:
			if (const auto a = std::get_if<VariableId>(&inst.arguments[0])) {
				variables[*a] = Get(inst.arguments[1]);
			}
			++programCounter;
			break;

		case InstructionType::beginif:
			if (const auto op = std::get_if<Operator>(&inst.arguments[1])) {
				//引数を取り出す
				const Number a = Get(inst.arguments[0]);
				const Number b = Get(inst.arguments[2]);

				//引数を比較する
				bool result = false;
				switch (*op) {
				case Operator::equal: result = a == b; break;
				case Operator::notEqual: result = a != b; break;
				case Operator::less: result = a < b; break;
				case Operator::lessEqual: result = a <= b; break;
				case Operator::greater: result = a > b; break;
				case Operator::greaterEqual: result = a >= b; break;
				}
				//比較結果がfalseならendifの位置にジャンプ
				if (!result) {
					programCounter = inst.jump;
					break;

				}
			}
			++programCounter;
			break;

		default:
			++programCounter;
			break;
		}

	}
	textWindow.Update(deltaTime);
}

/*
スクリプトエンジンを描画する
*/
void EventScriptEngine::Draw() {
	if (!isInitialized) {
		return;
	}
	textWindow.Draw();
}

/*
スクリプトの実行が完了したかを調べる

@retval true  実行完了
@retval false 実行中、またはスクリプトが読み込まれていない
*/
bool EventScriptEngine::IsFinished() const
{
	return isFinished;
}

/*
スクリプト変数に値を設定する

@param no    変数番号(0～初期化時に設定した最大数)
@param value 設定する値
*/
void EventScriptEngine::SetVariable(int no, double value)
{
	if (no < 0 || no >= static_cast<int>(variables.size())) {
		return;
	}
	variables[no] = value;
}

/*
スクリプト変数の値を取得する

@param no 変数番号(0～初期化時に設定した最大数)

@return no番の変数に設定されている値
*/
double EventScriptEngine::GetVariable(int no) const
{
	if (no < 0 || no >= static_cast<int>(variables.size())) {
		return 0;

	}
	return variables[no];
}