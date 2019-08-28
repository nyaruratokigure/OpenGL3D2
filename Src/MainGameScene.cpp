/*
@file MainGameScene.cpp
*/
#include "MainGameScene.h"
#include "StatusScene.h"
#include "GameOverScene.h"
#include "GLFWEW.h"

/*
シーンを初期化する
@retval true  初期化成功
@retval false 初期化失敗. ゲーム進行不可につき、プログラムを終了すること
*/
bool MainGameScene::Initialize()
{
	spriteRenderer.Init(1000, "Res/Sprite.vert", "Res/Sprite.frag");
	sprites.reserve(100);
	Sprite spr(Texture::Image2D::Create("Res/strike.tga"));
	spr.Scale(glm::vec2(2));
	sprites.push_back(spr);

	fontRenderer.Init(1000);
	fontRenderer.LoadFromFile("Res/font.fnt");

	return true;
}

/*
プレイヤーの入力を処理する
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
/*
シーンを更新する

@param deltaTime  前回の更新からの経過時間(秒)
*/
void MainGameScene::Update(float deltaTime)
{
	spriteRenderer.BeginUpdate();
	for (const Sprite& e : sprites) {
		spriteRenderer.AddVertices(e);
	}
	spriteRenderer.EndUpdate();

	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const float w = window.Width();
	const float h = window.Height();
	const float lineHeight = fontRenderer.LineHeight();
	fontRenderer.BeginUpdate();
	fontRenderer.AddString(glm::vec2(-w * 0.5f + 32, h * 0.5f - lineHeight), L"メインゲーム画面");
	fontRenderer.AddString(glm::vec2(-128, 0), L"アクションゲーム");
	fontRenderer.EndUpdate();
}

/*
シーンを描画する
*/
void MainGameScene::Render()
{
	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const glm::vec2 screenSize(window.Width(), window.Height());
	spriteRenderer.Draw(screenSize);
	fontRenderer.Draw(screenSize);
}
