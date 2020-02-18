/*
@file EventScene.cpp
*/
#include "EventScene.h"
#include "EventScript.h"

/*
�R���X�g���N�^

@param filename �X�N���v�g�t�@�C����
*/
EventScene::EventScene(const char* filename) :
	Scene("EventScane"), filename(filename)
{
}

/*
�C�x���g�V�[��������������
*/
bool EventScene::Initialize(){
	return EventScriptEngine::Instance().RunScript(filename.c_str());
}

/*
�C�x���g�V�[���̏�Ԃ��X�V����.

@param deltaTime  �O��̍X�V����̌o�ߎ���(�b).
*/
void EventScene::Update(float deltaTime)
{
	if (EventScriptEngine::Instance().IsFinished()) {
		SceneStack::Instance().Pop();
	}
}