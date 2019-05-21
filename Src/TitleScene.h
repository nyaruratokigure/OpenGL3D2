/*
@file TitleScene.h
*/
#ifndef TITLESCENE_H_INCLUDED
#define TITLESCENE_H_INCLUDED
#include"Scene.h"

/*
ƒ^ƒCƒgƒ‹‰æ–Ê
*/
class TitleScene :public Scene
{
public:
	TitleScene() :Scene("TitleScene") {}
	virtual ~TitleScene() = default;

	virtual bool Initialize() override { return true; }
	virtual void ProcessInput() override;
	virtual void Update(float) override {}
	virtual void Render() override {}
	virtual void Finalize() override {}
};

#endif // TITLESCENE_H_INCLUDED

