/*
@file TitleScene.cpp
*/
#include "TitleScene.h"
#include "MainGameScene.h"
#include "GLFWEW.h"

/*
�V�[��������������
@retval true  ����������
@retval false ���������s. �Q�[���i�s�s�ɂ��A�v���O�������I�����邱��
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

	//BGM���Đ�����
	bgm = Audio::Engine::Instance().Prepare("Res/Audio/Title.mp3");
	bgm->Play(Audio::Flag_Loop);

	return true;
}

/*
�v���C���[�̓��͂���������
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
�V�[�����X�V����

@param deltaTime  �O��̍X�V����̌o�ߎ���(�b)
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
	//fontRenderer.AddString(glm::vec2(-w * 0.5f + 32, h * 0.5f - lineHeight), L"�^�C�g�����");
	fontRenderer.AddString(glm::vec2(-175, -125), L"P  r  e  s  s   E  n  t  e  r   K  e  y");
	fontRenderer.EndUpdate(); 

	fontTimer -= deltaTime;

	//�V�[���؂�ւ��҂�
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
�V�[����`�悷��
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