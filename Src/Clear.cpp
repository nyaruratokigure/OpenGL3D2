/*
@file Clear.cpp
*/
#include "Clear.h"
#include "TitleScene.h"
#include "GLFWEW.h"

/*
�V�[��������������
@retval true  ����������
@retval false ���������s.�Q�[���i�s�s�ɂ��A�v���O�������I�����邱��
*/
bool Clear::Initialize()
{
	spriteRenderer.Init(1000, "Res/Sprite.vert", "Res/Sprite.frag");
	sprites.reserve(100);
	Sprite spr(Texture::Image2D::Create("Res/kiraa.tga"));
	spr.Scale(glm::vec2(2));
	sprites.push_back(spr);

	//BGM���Đ�����
	bgm = Audio::Engine::Instance().Prepare("Res/Audio/Clear.mp3");
	bgm->Play(Audio::Flag_Loop);

	fontRenderer.Init(1000);
	fontRenderer.LoadFromFile("Res/font.fnt");

	return true;
}

/*
�v���C���[�̓��͂���������
*/
void Clear::ProcessInput()
{
	GLFWEW::Window& window = GLFWEW::Window::Instance();
	if (timer <= 0 && (window.GetGamePad().buttonDown & GamePad::START)) {
		Audio::Engine::Instance().Prepare("Res/Audio/select.mp3")->Play();
		timer = 1.0f;
	}
}

/*
�V�[�����X�V����

@param deltaTime  �O��̍X�V����̌o�ߎ���(�b)
*/
void Clear::Update(float deltaTime)
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
	fontRenderer.AddString(glm::vec2(-w * 0.5f + 32, h * 0.5f - lineHeight), L"�N���A���");
	//fontRenderer.AddString(glm::vec2(-128, 0), L"�N���A�I");
	fontRenderer.EndUpdate();
	//�V�[���؂�ւ��҂�
	if (timer > 0) {
		timer -= deltaTime;
		if (timer <= 0) {
			bgm->Stop();
			SceneStack::Instance().Replace(std::make_shared<TitleScene>());
			return;
		}
	}
}

/*
�V�[����`�悷��
*/
void Clear::Render()
{
	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const glm::vec2 screenSize(window.Width(), window.Height());
	spriteRenderer.Draw(screenSize);
	fontRenderer.Draw(screenSize);
}
