/*
@file StatusScene.cpp
*/
#include "StatusScene.h"
#include "TitleScene.h"
#include "MainGameScene.h"
#include "GLFWEW.h"

/*
シーンを初期化する
@retval true  初期化成功
@retval false 初期化失敗.ゲーム進行不可につき、プログラムを終了すること
*/
//bool StatusScene::Initialize()
//{
//	spriteRenderer.Init(1000, "Res/Sprite.vert", "Res/Sprite.frag");
//	sprites.reserve(100);
//	Sprite spr(Texture::Image2D::Create("Res/strike.tga"));
//	spr.Scale(glm::vec2(2));
//	sprites.push_back(spr);
//
//	fontRenderer.Init(1000);
//	fontRenderer.LoadFromFile("Res/font.fnt");
//
//	return true;
//}

/*
プレイヤーの入力を処理する
*/
void StatusScene::ProcessInput()
{
	GLFWEW::Window& window = GLFWEW::Window::Instance();
	if (timer <= 0 && (window.GetGamePad().buttonDown & GamePad::START)) {
		Audio::Engine::Instance().Prepare("Res/Audio/select.mp3")->Play();
		timer = 1.0f;
	}
}

/*
シーンを更新する

@param deltaTime  前回の更新からの経過時間(秒)
*/
//void StatusScene::Update(float deltaTime)
//{
//	spriteRenderer.BeginUpdate();
//	for (const Sprite& e : sprites) {
//		spriteRenderer.AddVertices(e);
//	}
//	spriteRenderer.EndUpdate();
//
//	const GLFWEW::Window& window = GLFWEW::Window::Instance();
//	const float w = window.Width();
//	const float h = window.Height();
//	const float lineHeight = fontRenderer.LineHeight();
//	fontRenderer.BeginUpdate();
//	fontRenderer.AddString(glm::vec2(-w * 0.5f + 32, h * 0.5f - lineHeight), L"ステータス画面");
//	fontRenderer.AddString(glm::vec2(-128, 0), L"アクションゲーム");
//	fontRenderer.EndUpdate();
//	//シーン切り替え待ち
//	if (timer > 0) {
//		timer -= deltaTime;
//		if (timer <= 0) {
//			SceneStack::Instance().Replace(std::make_shared<MainGameScene>());
//			return;
//		}
//	}
//}

/*
シーンを描画する
*/
//void StatusScene::Render()
//{
//	const GLFWEW::Window& window = GLFWEW::Window::Instance();
//	const glm::vec2 screenSize(window.Width(), window.Height());
//	spriteRenderer.Draw(screenSize);
//	fontRenderer.Draw(screenSize);
//}
