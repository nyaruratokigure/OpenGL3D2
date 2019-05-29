/*
@file StatusScene.cpp
*/
#include "StatusScene.h"
#include "GLFWEW.h"

/*
ƒvƒŒƒCƒ„[‚Ì“ü—Í‚ğˆ—‚·‚é
*/
void StatusScene::ProcessInput()
{
	GLFWEW::Window& window = GLFWEW::Window::Instance();
	if (window.GetGamePad().buttonDown& GamePad::START) {
		SceneStack::Instance().Pop();
	}
}