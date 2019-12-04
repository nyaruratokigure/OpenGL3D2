/*
@file StatusScene.cpp
*/
#include "StatusScene.h"
#include "TitleScene.h"
#include "MainGameScene.h"
#include "GLFWEW.h"

/*
�V�[��������������
@retval true  ����������
@retval false ���������s.�Q�[���i�s�s�ɂ��A�v���O�������I�����邱��
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
�v���C���[�̓��͂���������
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
�V�[�����X�V����

@param deltaTime  �O��̍X�V����̌o�ߎ���(�b)
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
//	fontRenderer.AddString(glm::vec2(-w * 0.5f + 32, h * 0.5f - lineHeight), L"�X�e�[�^�X���");
//	fontRenderer.AddString(glm::vec2(-128, 0), L"�A�N�V�����Q�[��");
//	fontRenderer.EndUpdate();
//	//�V�[���؂�ւ��҂�
//	if (timer > 0) {
//		timer -= deltaTime;
//		if (timer <= 0) {
//			SceneStack::Instance().Replace(std::make_shared<MainGameScene>());
//			return;
//		}
//	}
//}

/*
�V�[����`�悷��
*/
//void StatusScene::Render()
//{
//	const GLFWEW::Window& window = GLFWEW::Window::Instance();
//	const glm::vec2 screenSize(window.Width(), window.Height());
//	spriteRenderer.Draw(screenSize);
//	fontRenderer.Draw(screenSize);
//}
