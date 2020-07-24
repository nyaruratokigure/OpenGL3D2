/*
@file ClearScene.h
*/
#ifndef CLEAR_H_INCLUDED
#define CLEAR_H_INCLUDED
#include"Scene.h"
#include "Sprite.h"
#include "Font.h"
#include "Audio/Audio.h"

/*
ƒNƒŠƒA‰æ–Ê
*/
class ClearScene :public Scene
{
public:
	ClearScene():Scene("Clear") {}
	virtual ~ClearScene() = default;

	virtual bool Initialize() override;
	virtual void ProcessInput() override;
	virtual void Update(float) override;
	virtual void Render() override;
	virtual void Finalize() override {}

private:
	std::vector<Sprite> sprites;
	SpriteRenderer spriteRenderer;
	FontRenderer fontRenderer;
	float timer = 5;
	Audio::SoundPtr bgm;
};

#endif // CLEAR_H_INCLUDED
