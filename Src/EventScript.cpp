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
�X�N���v�g���߂̈����ɒl��ݒ肷��

@param arag �X�N���v�g���߂̈���
@param str �ݒ肷��l���܂ޕ�����
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
�X�N���v�g���߂̈����Ɋ֌W���Z�q��ݒ肷��

@param arg �X�N���v�g���߂̈���
@param str �ݒ肷��l���܂ޕ�����
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
�X�N���v�g���߂̈�������l���擾����

@param arg �X�N���v�g���߂̈���

@return �������璼�ځE�Ԑڂɓ���ꂽ�l
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
�X�N���v�g�G���W���̃V���O���g���E�C���X�^���X���擾����

@return �X�N���v�g�G���W���̃V���O���g���E�C���X�^���X
*/
EventScriptEngine& EventScriptEngine::Instance()
{
	static EventScriptEngine instance;
	return instance;
}

/*
�X�N���v�g�G���W��������������

@retval true  ����������
@retval false ���������s
*/
bool EventScriptEngine::Init()
{
	if (isInitialized) {
		std::cerr << "[�G���[]EventScriptEngine�͊��ɏ���������Ă��܂�\n";
		return false;
	}

	filename.reserve(256);
	script.reserve(2048);
	if (!textWindow.Init("Res/TextWindow.tga",
		glm::vec2(0, -248), glm::vec2(48, 32), glm::vec2(0))) {
		std::cerr << "[�G���[]" << __func__ << ":�X�N���v�g�G���W���̏������Ɏ��s\n";
		return false;
	}
	variables.resize(100, 0.0);
	isInitialized = true;
	return true;
}

/*
�C�x���g�E�X�N���v�g�����s����

@param filename �X�N���v�g�E�t�@�C����

@retval true  ���s�ɐ���
@retval false ���s�Ɏ��s
*/
bool EventScriptEngine::RunScript(const char*filename)
{
	if (!isInitialized) {
		return false;
	}

	//�X�N���v�g�t�@�C����ǂݍ��݁A�e�L�X�g�E�B���h�E�\���̂��߂�wchar_t�^�ɕϊ�����
	std::ifstream ifs(filename);
	if (!ifs) {
		std::cerr << "[�G���[]" << __func__ << ":�X�N���v�g�t�@�C��" << filename <<
			"��ǂݍ��߂܂���\n";
		return false;
	}
	setlocale(LC_CTYPE, "ja-JP");

	script.clear();//�ȑO�̃X�N���v�g������
	size_t lineCount = 0;
	std::string line;
	char buf[1000];
	char a[20], b[20],c[20],op[20];//�X�N���v�g�����p
	std::vector<size_t>jumpStack;
	while (std::getline(ifs, line)) {
		//�擪�̗]������������
		line.erase(0, line.find_first_not_of("\t\n"));
		++lineCount;

		Instruction inst;
		//print���߂�ǂݍ���
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

		//�l�����Z���߂�ǂݎ��
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

		//������߂�ǂݎ��
		n = sscanf(line.c_str(), "[%19[^]]]=%19s", a, b);
		if (n >= 2) {
			inst.type = InstructionType::assign;
			inst.arguments[0] = static_cast<VariableId>(atoi(a));
			Set(inst.arguments[1], b);
			script.push_back(inst);
			continue;
		}

		//if���߂�ǂݎ��
		n = sscanf(line.c_str(), "if %19s %19s %19s", a, op, b);
		if (n >= 3) {
			inst.type = InstructionType::beginif;
			Set(inst.arguments[0], a);
			SetOperator(inst.arguments[1], op);
			Set(inst.arguments[2], b);
			script.push_back(inst);

			//�W�����v���ݒ�ł���悤��if���߂̈ʒu���L�^
			jumpStack.push_back(script.size() - 1);
			continue;
		}

		//endif���߂�ǂݎ��
		if (strncmp(line.c_str(), "endif", 5) == 0) {
			if (jumpStack.empty()) {
				std::cerr << "[�G���[]" << __func__ << "endif���������܂�(" <<
				lineCount << "�s��)\n";
				continue;
			}
			const size_t p = jumpStack.back();
			jumpStack.pop_back();
			script[p].jump = script.size();
			continue;
		}
	}

	programCounter = 0;

	//�p�����[�^��ݒ�
	isFinished = false;
	this->filename = filename;

	std::cout << "[INFO]" << __func__ << ":�X�N���v�g�t�@�C��" << filename << "�����s\n";
	return true;
}

/*
�X�N���v�g�G���W���̏�Ԃ��X�V����

@param deltaTime �O��̍X�V����̌o�ߎ���(�b)
*/
void EventScriptEngine::Update(float deltaTime)
{
	if (!isInitialized) {
		return;
	}

	//�X�N���v�g���ݒ�A�܂��͎��s�I���Ȃ牽�����Ȃ�
	if (script.empty() || isFinished) {
		return;

	}
	//���߂����s����
	//���s�𒆒f����ꍇ��yield�ϐ���true��ݒ�
	for (bool yield = false; !yield; ) {
		//���s�ʒu(programCounter)���X�N���v�g�̖��ߐ��ȏ�Ȃ���s����
		if (programCounter >= script.size()) {
			isFinished = true;
			break;
		}

		const auto& inst = script[programCounter];
		switch (inst.type) {
		case InstructionType::print:
			if (!textWindow.IsOpen()) {
				//�E�B���h�E�����Ă���̂ŕ��͂�ݒ�
				const auto p = std::get_if<Text>(&inst.arguments[0]);
				if (!p) {
					std::cerr << "[�G���[]" << __func__ <<
						"print���߂̈�����Text�^�łȂ��Ă͂Ȃ�܂���\n";
					++programCounter;
					break;
				}
				textWindow.Open(p->c_str());
			}
			else {
				//�E�B���h�E���J���Ă���\���I����҂�. �\�����I��������A�L�[���͂�҂�
				//�L�[���͂���������A�E�B���h�E����āA���̖��߂̏����֐i��
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
				//���������o��
				const Number a = Get(inst.arguments[0]);
				const Number b = Get(inst.arguments[2]);

				//�������r����
				bool result = false;
				switch (*op) {
				case Operator::equal: result = a == b; break;
				case Operator::notEqual: result = a != b; break;
				case Operator::less: result = a < b; break;
				case Operator::lessEqual: result = a <= b; break;
				case Operator::greater: result = a > b; break;
				case Operator::greaterEqual: result = a >= b; break;
				}
				//��r���ʂ�false�Ȃ�endif�̈ʒu�ɃW�����v
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
�X�N���v�g�G���W����`�悷��
*/
void EventScriptEngine::Draw() {
	if (!isInitialized) {
		return;
	}
	textWindow.Draw();
}

/*
�X�N���v�g�̎��s�������������𒲂ׂ�

@retval true  ���s����
@retval false ���s���A�܂��̓X�N���v�g���ǂݍ��܂�Ă��Ȃ�
*/
bool EventScriptEngine::IsFinished() const
{
	return isFinished;
}

/*
�X�N���v�g�ϐ��ɒl��ݒ肷��

@param no    �ϐ��ԍ�(0�`���������ɐݒ肵���ő吔)
@param value �ݒ肷��l
*/
void EventScriptEngine::SetVariable(int no, double value)
{
	if (no < 0 || no >= static_cast<int>(variables.size())) {
		return;
	}
	variables[no] = value;
}

/*
�X�N���v�g�ϐ��̒l���擾����

@param no �ϐ��ԍ�(0�`���������ɐݒ肵���ő吔)

@return no�Ԃ̕ϐ��ɐݒ肳��Ă���l
*/
double EventScriptEngine::GetVariable(int no) const
{
	if (no < 0 || no >= static_cast<int>(variables.size())) {
		return 0;

	}
	return variables[no];
}