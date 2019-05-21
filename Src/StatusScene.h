/*
@file StatusScene.h
*/
#ifndef STATUSSCENE_H_INCLUDED
#define STATUSSCENE_H_INCLUDED
#include"Scene.h"

/*
ステータス画面
*/
class StatusScene :public Scene
{
public:
	StatusScene() :Scene("StatusScene") {}
	virtual ~StatusScene() = default;

	virtual bool Initialize() override { return true; }
	virtual void ProcessInput() override;
	virtual void Update(float) override {}
	virtual void Render() override {}
	virtual void Finalize() override {}
};

#endif // STATUSSCENE_H_INCLUDED
