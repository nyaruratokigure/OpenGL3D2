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
�C�x���g�X�N���v�g����N���X
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

	//���߂̎��
	enum class InstructionType {
		nop,//�������Ȃ��_�~�[����
		print,//���͂�\��
		expression,//���Z��
		assign,//�ϐ��ɒl��ݒ�
		beginif,//if���߂̊J�n
		endif,//if���߂̏I�[
	};

	//�֌W���Z�q
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

	//�����̌^
	using Text = std::wstring; //����
	using VariableId = int;    //�ϐ�(�̔ԍ�)
	using Number = double;     //���l
	using Argument = std::variant<Text, VariableId, Number, Operator>;

	void Set(Argument&, const char*);
	void SetOperator(Argument&, const char*);
	Number Get(const Argument&)const;

	//�X�N���v�g���ߌ^
	struct Instruction {
		InstructionType type = InstructionType::nop;
		Argument arguments[4];
		size_t jump = 0;//if���߂̃W�����v��
	};

	std::string filename;
	std::vector<Instruction> script;
	std::vector<double>variables;//�X�N���v�g�ϐ�
	size_t programCounter = 0; //���Ɏ��s���閽�߂̈ʒu

	bool isInitialized = false;//�G���W��������������Ă����true
	bool isFinished = false;//�X�N���v�g�̎��s���I��������true

	TextWindow textWindow;
};

#endif // EVENTSCRIPT_H_INCLUDED