/*
@file EventScript.h
*/
#ifndef EVENTSCRIPT_H_INCLUDED
#define EVENTSCRIPT_H_INCLUDED
#include "TextWindow.h"
#include <string>
#include <variant>
#include <vector>

/*
イベントスクリプト制御クラス
*/
class EventScriptEngine
{
public:
	static EventScriptEngine& Instance();

	bool Init();
	bool RunScript(const char* filename);
	void Update(float deltaTime);
	void Draw();

	bool IsFinished() const;

	void SetVariable(int no, double value);
	double GetVariable(int no) const;

private:
	EventScriptEngine() = default;
	~EventScriptEngine() = default;
	EventScriptEngine(const EventScriptEngine&) = delete;
	EventScriptEngine& operator=(const EventScriptEngine&) = delete;

	//命令の種類
	enum class InstructionType {
		nop,//何もしないダミー命令
		print,//文章を表示
		expression,//演算式
		assign,//変数に値を設定
		beginif,//if命令の開始
		endif,//if命令の終端
	};

	//関係演算子
	enum class Operator {
		equal,        //==
		notEqual,     //!=
		less,         //<
		lessEqual,    //<=
		greater,      //>
		greaterEqual, //>=

		add, //+
		sub, //-
		mul, //*
		div, // /
	};

	//引数の型
	using Text = std::wstring; //文章
	using VariableId = int;    //変数(の番号)
	using Number = double;     //数値
	using Argument = std::variant<Text, VariableId, Number, Operator>;

	void Set(Argument&, const char*);
	void SetOperator(Argument&, const char*);
	Number Get(const Argument&)const;

	//スクリプト命令型
	struct Instruction {
		InstructionType type = InstructionType::nop;
		Argument arguments[4];
		size_t jump = 0;//if命令のジャンプ先
	};

	std::string filename;
	std::vector<Instruction> script;
	std::vector<double>variables;//スクリプト変数
	size_t programCounter = 0; //次に実行する命令の位置

	bool isInitialized = false;//エンジンが初期化されていればtrue
	bool isFinished = false;//スクリプトの実行が終了したらtrue

	TextWindow textWindow;
};

#endif // EVENTSCRIPT_H_INCLUDED