/*
@file GameOverScene.cpp
*/
#include "GameOverScene.h"
#include "TitleScene.h"
#include "GLFWEW.h"

/*
ƒvƒŒƒCƒ„[‚Ì“ü—Í‚ğˆ—‚·‚é
*/
void GameOverScene::ProcessInput()
{
	GLFWEW::Window& window = GLFWEW::Window::Instance();
	if (window.GetGamePad().buttonDown& GamePad::START) {
		SceneStack::Instance().Replace(std::make_shared<TitleScene>());
	}
}