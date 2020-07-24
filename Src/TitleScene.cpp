/*
@file TitleScene.cpp
*/
#include "TitleScene.h"
#include "MainGameScene.h"
#include "GLFWEW.h"

/*
シーンを初期化する
@retval true  初期化成功
@retval false 初期化失敗. ゲーム進行不可につき、プログラムを終了すること
*/
bool TitleScene::Initialize()
{
	spriteRenderer.Init(1000, "Res/Sprite.vert", "Res/Sprite.frag");
	sprites.reserve(100);
	Sprite spr(Texture::Image2D::Create("Res/TitleBg.tga"));
	spr.Scale(glm::vec2(2));
	sprites.push_back(spr);

	fontRenderer.Init(1000);
	fontRenderer.LoadFromFile("Res/font.fnt");

	//BGMを再生する
	bgm = Audio::Engine::Instance().Prepare("Res/Audio/Title.mp3");
	bgm->Play(Audio::Flag_Loop);

	return true;
}

/*
プレイヤーの入力を処理する
*/
void TitleScene::ProcessInput()
{
	GLFWEW::Window& window = GLFWEW::Window::Instance();
	if (timer <= 0 && (window.GetGamePad().buttonDown & GamePad::START)) {
		Audio::Engine::Instance().Prepare("Res/Audio/TitleSE.wav")->Play();
		timer = 3.0f;
	}
}

/*
シーンを更新する

@param deltaTime  前回の更新からの経過時間(秒)
*/
void TitleScene::Update(float deltaTime)
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
	//fontRenderer.AddString(glm::vec2(-w * 0.5f + 32, h * 0.5f - lineHeight), L"タイトル画面");
	fontRenderer.AddString(glm::vec2(-175, -125), L"P  r  e  s  s   E  n  t  e  r   K  e  y");
	fontRenderer.EndUpdate(); 

	fontTimer -= deltaTime;

	//シーン切り替え待ち
	if (timer > 0) {
		timer -= deltaTime;
		if (timer <= 0) {
			bgm->Stop();
			SceneStack::Instance().Replace(std::make_shared<MainGameScene>());
			return;
		}
	}
}

/*
シーンを描画する
*/
void TitleScene::Render()
{
	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const glm::vec2 screenSize(window.Width(), window.Height());
	spriteRenderer.Draw(screenSize);
	if (timer > 0) {
		if ((int)(fontTimer * 10) % 2) {
			fontRenderer.Draw(screenSize);
		}
	}
	else {
		if ((int)fontTimer % 2) {
			fontRenderer.Draw(screenSize);
		}
	}
}