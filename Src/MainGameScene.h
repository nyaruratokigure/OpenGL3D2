/*
@file MainGameScene.h
*/
#ifndef MAINGAMESCENE_H_INCLUDED
#define MAINGAMESCENE_H_INCLUDED
#include "Scene.h"
#include "Font.h"
#include "Sprite.h"
#include "Mesh.h"
#include "Terrain.h"
#include "Actor.h"
#include "PlayerActor.h"
#include "Audio/Audio.h"
#include "JizoActor.h"
#include "Light.h"
#include "FramebufferObject.h"
#include "Particle.h"
#include "TextWindow.h"
#include "EnemyActor.h"
#include <random>
#include <vector>

/*
メインゲーム画面
*/
class MainGameScene :public Scene
{
public:
	MainGameScene() :Scene("MainGameScene") {}
	virtual ~MainGameScene() = default;

	virtual bool Initialize() override ;
	virtual void ProcessInput() override;
	virtual void Update(float)override;
	virtual void Render() override;
	virtual void Finalize() override {}

	bool HandleJizoEffects(int id, const glm::vec3& pos);

private: 
	void RenderMesh(Mesh::DrawType);
	bool hitEffect = false;
	bool flag = false;
	std::mt19937 rand;
	int particleID = 0;
	int jizoCount = 4;
	int jizoId = -1; ///<現在戦闘中のお地蔵様のID
	bool achivements[4] = { false, false, false, false }; ///<敵討伐状態
	std::vector<Sprite> sprites;
	SpriteRenderer spriteRenderer;
	FontRenderer fontRenderer;
	Audio::SoundPtr bgm;
	Mesh::Buffer meshBuffer;
	Terrain::HeightMap heightMap;
	float timer = 0;

	struct Camera {
		glm::vec3 target = glm::vec3(100, 0, 100);
		glm::vec3 position = glm::vec3(100, 50, 150);
		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 velocity = glm::vec3(0);

		//画面パラメータ
		float width = 1280; ///< 画面の幅(ピクセル数).
		float height = 720; ///< 画面の高さ(ピクセル数).
		float near = 1;     ///< 最小Z値(メートル).
		float far = 500;    ///< 最大Z値(メートル).
		
		//カメラパラメータ
		float fNumber = 1.4f;            ///< エフ・ナンバー = カメラのF値.
		float fov = glm::radians(60.0f); ///< フィールド・オブ・ビュー = カメラの視野角(ラジアン).
		float sensorSize = 36.0f;        ///< センサー・サイズ = カメラのセンサーの横幅(ミリ).
		
		//Update関数で計算するパラメータ
		float focalLength = 50.0f;   ///< フォーカル・レングス = 焦点距離(ミリ).
		float aperture = 20.0f;      ///< アパーチャ = 開口(ミリ).
		float focalPlane = 10000.0f; ///< フォーカル・プレーン = ピントの合う距離.
		
		void Update(const glm::mat4& matView);

	};
	Camera camera;

	PlayerActorPtr player;
	ActorList enemies;
	ActorList trees;
	ActorList objects;
	ActorList marks;
	EnemyActorPtr enep;


	LightBuffer lightBuffer;
	ActorList lights;

	ParticleSystem particleSystem;
	TextWindow textWindow;

	FrameBufferObjectPtr fboMain;
	FrameBufferObjectPtr fboDepthOfField;
	FrameBufferObjectPtr fboBloom[6][2];
	FrameBufferObjectPtr fboShadow;
};

#endif // MAINGAMESCENE_H_INCLUDED

