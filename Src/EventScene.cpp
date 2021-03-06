/*
@file EventScene.cpp
*/
#include "EventScene.h"
#include "EventScript.h"

/*
コンストラクタ

@param filename スクリプトファイル名
*/
EventScene::EventScene(const char* filename) :
	Scene("EventScane"), filename(filename)
{
}

/*
イベントシーンを初期化する
*/
bool EventScene::Initialize(){
	return EventScriptEngine::Instance().RunScript(filename.c_str());
}

/*
イベントシーンの状態を更新する.

@param deltaTime  前回の更新からの経過時間(秒).
*/
void EventScene::Update(float deltaTime)
{
	if (EventScriptEngine::Instance().IsFinished()) {
		SceneStack::Instance().Pop();
	}
}