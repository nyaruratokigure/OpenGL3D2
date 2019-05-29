/*
@file MainGameScene.cpp
*/
#include "MainGameScene.h"
#include "StatusScene.h"
#include "GameOverScene.h"
#include "GLFWEW.h"

/*
ƒvƒŒƒCƒ„[‚Ì“ü—Í‚ğˆ—‚·‚é
*/
void MainGameScene::ProcessInput()
{
	if (!flag) {

		GLFWEW::Window& window = GLFWEW::Window::Instance();
		if (window.GetGamePad().buttonDown& GamePad::START) {
			flag = true;
			SceneStack::Instance().Push(std::make_shared<StatusScene>());
		}
	}
	else {
		GLFWEW::Window& window = GLFWEW::Window::Instance();
		if (window.GetGamePad().buttonDown& GamePad::START) {
			SceneStack::Instance().Replace(std::make_shared<GameOverScene>());
		}
	}
}