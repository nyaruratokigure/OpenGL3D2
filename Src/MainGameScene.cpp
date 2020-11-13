/*
@file MainGameScene.cpp
*/
#include "MainGameScene.h"
#include "TitleScene.h"
#include "StatusScene.h"
#include "GameOverScene.h"
#include "ClearScene.h"
#include "GLFWEW.h"
#include "SkeletalMeshActor.h"
#include "EventScene.h"
#include "EnemyActor.h "
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <random>
#include <sstream>

/*
衝突を解決する

@param a 衝突したアクターその１
@param b 衝突したアクターその２
@param p 衝突位置
*/
void PlayerCollisionHandler(const ActorPtr& a, const ActorPtr& b, const glm::vec3& p)
{
	const glm::vec3 v = a->colWorld.s.center - p;
	// 衝突位置との距離が近すぎないか調べる
	if (dot(v, v) > FLT_EPSILON) {
		// aをbに重ならない位置まで移動.
		const glm::vec3 vn = normalize(v);
		float radiusSum = a->colWorld.s.r;
		switch (b->colWorld.type) {
		case Collision::Shape::Type::sphere: radiusSum += b->colWorld.s.r; break;
		case Collision::Shape::Type::capsule: radiusSum += b->colWorld.c.r; break;
		}
		const float distance = radiusSum - glm::length(v) + 0.01f;
		a->position += vn * distance;
		a->colWorld.s.center += vn * distance;
		if (a->velocity.y < 0 && vn.y >= glm::cos(glm::radians(60.0f))) {
			a->velocity.y = 0;
		}
	}
	else {
		// 移動を取り消す(距離が近すぎる場合の例外処理)
		const float deltaTime = static_cast<float>(GLFWEW::Window::Instance().DeltaTime());
		const glm::vec3 deltaVelocity = a->velocity * deltaTime;
		a->position -= deltaVelocity;
		a->colWorld.s.center -= deltaVelocity;

	}
}

/*
シーンを初期化する
@retval true  初期化成功
@retval false 初期化失敗. ゲーム進行不可につき、プログラムを終了すること
*/
bool MainGameScene::Initialize()
{
	spriteRenderer.Init(1000, "Res/Sprite.vert", "Res/Sprite.frag");
	sprites.reserve(100);
	Sprite spr(Texture::Image2D::Create("Res/kuro.tga"));
	spr.Scale(glm::vec2(2));
	sprites.push_back(spr);

	//BGMを再生する
	bgm = Audio::Engine::Instance().Prepare("Res/Audio/coldfish.mp3");
	bgm->Play(Audio::Flag_Loop);

	fontRenderer.Init(1000);
	fontRenderer.LoadFromFile("Res/font.fnt");

	//テキストウィンドウを表示
	textWindow.Init("Res/TextWindow.tga",
		glm::vec2(0, -248), glm::vec2(48, 32), glm::vec2(0));
	/*textWindow.Open(
		L"地蔵を悪い鬼から救おう！\n(地蔵に触れてみよう)\nWASD :移動\nJ :攻撃");*/

	meshBuffer.Init(1'000'000 * sizeof(Mesh::Vertex), 3'000'000 * sizeof(GLushort));
	lightBuffer.Init(1);
	lightBuffer.BindToShader(meshBuffer.GetStaticMeshShader());
	lightBuffer.BindToShader(meshBuffer.GetTerrainShader());
	lightBuffer.BindToShader(meshBuffer.GetWaterShader());

	meshBuffer.LoadMesh("Res/red_pine_tree.gltf");
	meshBuffer.LoadMesh("Res/jizo_statue.gltf");
	meshBuffer.LoadSkeletalMesh("Res/bikuni.gltf");
	meshBuffer.LoadSkeletalMesh("Res/oni_small.gltf");
	meshBuffer.LoadMesh("Res/wall_stone.gltf");
	meshBuffer.LoadMesh("Res/exclamation.gltf");

	//パーティクル・システムを初期化する
	particleSystem.Init(1000);

	//FBOを作成する
	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	fboMain = FrameBufferObject::Create(window.Width(), window.Height(), GL_RGBA16F);
	Mesh::FilePtr rt = meshBuffer.AddPlane("RenderTarget");
	if (rt) {
		rt->materials[0].program = Shader::Program::Create(
			"Res/DepthOfField.vert", "Res/DepthOfField.frag");
		rt->materials[0].texture[0] = fboMain->GetColorTexture();
		rt->materials[0].texture[1] = fboMain->GetDepthTexture();
	}
	if (!rt || !rt->materials[0].program) {
		return false;
	}

	//DoF描画用のFBOを作る
	fboDepthOfField = FrameBufferObject::Create(window.Width(), window.Height(), GL_RGBA16F);

	//元解像度の縦横1/2(面積が1/4)の大きさのブルーム用FBOを作る
	int w = window.Width();
	int h = window.Height();
	for (int j = 0; j < sizeof(fboBloom) / sizeof(fboBloom[0]); ++j) {
		w /= 2;
		h /= 2;
		for (int i = 0; i < sizeof(fboBloom[0]) / sizeof(fboBloom[0][0]); ++i) {
			fboBloom[j][i] = FrameBufferObject::Create(w, h, GL_RGBA16F, FrameBufferType::colorOnly);
			if (!fboBloom[j][i]) {
				return false;
			}
		}
	}

	// ブルーム・エフェクト用の平面ポリゴンメッシュを作成する.
	if (Mesh::FilePtr mesh = meshBuffer.AddPlane("BrightPassFilter")) {
		Shader::ProgramPtr p = Shader::Program::Create("Res/Simple.vert", "Res/BrightPassFilter.frag");
		p->Use();
		p->SetViewProjectionMatrix(glm::mat4(1));
		mesh->materials[0].program = p;
	}
	if (Mesh::FilePtr mesh = meshBuffer.AddPlane("NormalBlur")) {
		Shader::ProgramPtr p = Shader::Program::Create("Res/Simple.vert", "Res/NormalBlur.frag");
		p->Use();
		p->SetViewProjectionMatrix(glm::mat4(1));
		mesh->materials[0].program = p;
	}
	if (Mesh::FilePtr mesh = meshBuffer.AddPlane("Simple")) {
		Shader::ProgramPtr p = Shader::Program::Create("Res/Simple.vert", "Res/Simple.frag");
		p->Use();
		p->SetViewProjectionMatrix(glm::mat4(1));
		mesh->materials[0].program = p;
	}
	if (glGetError()) {
		std::cout << "[エラー]" << __func__ << ": ブルーム用メッシュの作成に失敗.\n";
		return false;
	}

	//デプスシャドウマッピング用のFBOを作成する
	{
		fboShadow = FrameBufferObject::Create(
			4096, 4096, GL_NONE, FrameBufferType::depthOnly);
		if (glGetError()) {
			std::cout << "[エラー]" << __func__ << ":シャドウ用FBOの作成に失敗\n";
			return false;
		}
		//sampler2DShadowの比較モードを設定する
		glBindTexture(GL_TEXTURE_2D, fboShadow->GetDepthTexture()->Get());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	//ハイトマップを作成する
	if (!heightMap.LoadFromFile("Res/Terrain.tga", 20.0f, 0.5f)) {
		return false;
	}
	if (!heightMap.CreateMesh(meshBuffer, "Terrain")) {
		return false;
	}
	if (!heightMap.CreateWaterMesh(meshBuffer, "Water", -1)) { //水面の高さ
		return false;
	}

	glm::vec3 startPos(100, 0, 100);
	startPos.y = heightMap.Height(startPos);

	player = std::make_shared<PlayerActor>(&heightMap, meshBuffer, startPos);

	rand.seed(0);

	// ライトを配置
	const int lightRangeMin = 80;
	const int lightRangeMax = 120;
	lights.Add(std::make_shared<DirectionalLightActor>(
		"DirectionalLight", glm::vec3(1.0f, 0.94f, 0.91f), glm::normalize(glm::vec3(1, -2, -1))));
	for (int i = 0; i < 50; ++i) {
		glm::vec3 color = glm::vec3(1, 0.8f, 0.5f)*20.0f;//ポイントライトの色＊明るさ
		glm::vec3 position(0);
		position.x = static_cast<float>(std::uniform_int_distribution<>(80, 120)(rand));
		position.z = static_cast<float>(std::uniform_int_distribution<>(80, 120)(rand));
		position.y = heightMap.Height(position) + 5;
		lights.Add(std::make_shared<PointLightActor>("PointLight", color, position));
	}
	for (int i = 0; i < 50; ++i) {
		glm::vec3 color = glm::vec3(1, 2, 3) * 2.0f;//スポットライトの色＊明るさ
		glm::vec3 direction(glm::normalize(glm::vec3(0.25f, -1, 0.25f)));
		glm::vec3 position(0);
		position.x = static_cast<float>(std::uniform_int_distribution<>(lightRangeMin, lightRangeMax)(rand));
		position.z = static_cast<float>(std::uniform_int_distribution<>(lightRangeMin, lightRangeMax)(rand));
		position.y = heightMap.Height(position) + 5;
		lights.Add(std::make_shared<SpotLightActor>("SpotLight", color, position, direction
			, glm::radians(20.0f), glm::radians(15.0f)));
	}
	lights.Update(0);
	lightBuffer.Update(lights, glm::vec3(0.1f, 0.05f, 0.15f));
	heightMap.UpdateLightIndex(lights);


	// 石壁を配置
	/*{
		const Mesh::FilePtr meshStoneWall = meshBuffer.GetFile("Res/wall_stone.gltf");
		glm::vec3 position = startPos + glm::vec3(3, 0, 3);
		position.y = heightMap.Height(position);
		StaticMeshActorPtr p = std::make_shared<StaticMeshActor>(
			meshStoneWall, "StoneWall", 100, position, glm::vec3(0, 0.5f, 0));
		p->colLocal = Collision::CreateOBB(glm::vec3(0, 0, 0),
			glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, -1), glm::vec3(2, 2, 0.5f));
		objects.Add(p);
	}*/

	//お地蔵様を配置
	for (int i = 0; i < 4; ++i) {
		glm::vec3 position(0);
		position.x = static_cast<float>(std::uniform_int_distribution<>/*(75, 125)*/(85, 115)(rand));
		position.z = static_cast<float>(std::uniform_int_distribution<>/*(75, 125)*/(85, 115)(rand));
		position.y = heightMap.Height(position);
		glm::vec3 rotation(0);
		rotation.y = std::uniform_real_distribution<float>(0.0f, 3.14f * 2.0f)(rand);
		JizoActorPtr p = std::make_shared<JizoActor>(
			meshBuffer.GetFile("Res/jizo_statue.gltf"), position, i, this);
		p->scale = glm::vec3(3); //見つけやすいように拡大
		objects.Add(p);
	}

	//敵を配置
	{
		const size_t oniCount = 30;
		enemies.Reserve(oniCount);
#if 0
		for (size_t i = 0; i < oniCount; ++i) {
			//敵の位置を(50,50)-(150,150)の範囲からランダムに選択
			glm::vec3 position(0);
			position.x = std::uniform_real_distribution<float>(50, 150)(rand);
			position.z = std::uniform_real_distribution<float>(50, 150)(rand);
			position.y = heightMap.Height(position);
			//敵の向きをランダムに選択
			glm::vec3 rotation(0);
			rotation.y = std::uniform_real_distribution<float>(0, glm::pi<float>() * 2)(rand);

			const Mesh::SkeletalMeshPtr mesh = meshBuffer.GetSkeletalMesh("oni_small");
			SkeletalMeshActorPtr p = std::make_shared<SkeletalMeshActor>(
				mesh, "Kooni", 13, position, rotation);
			p->GetMesh()->Play("Wait");

			p->colLocal = Collision::CreateCapsule(
				glm::vec3(0, 0.5f, 0), glm::vec3(0, 1, 0), 0.5f);
			enemies.Add(p);
		}
#endif
	}

	//木を配置
	{
		const size_t treeCount = 30;
		trees.Reserve(treeCount);
		const Mesh::FilePtr mesh = meshBuffer.GetFile("Res/red_pine_tree.gltf");
		for (size_t i = 0; i < treeCount; ++i) {
			//敵の位置を(50,50)-(150,150)の範囲からランダムに選択
			glm::vec3 position(0);
			position.x = std::uniform_real_distribution<float>(50, 150)(rand);
			position.z = std::uniform_real_distribution<float>(50, 150)(rand);
			position.y = heightMap.Height(position);
			//敵の向きをランダムに選択
			glm::vec3 rotation(0);
			rotation.y = std::uniform_real_distribution<float>(0, glm::pi<float>() * 2)(rand);
			StaticMeshActorPtr p = std::make_shared<StaticMeshActor>(
				mesh, "trees", 13, position, rotation);
			p->colLocal = Collision::CreateCapsule(
				glm::vec3(0, 0, 0), glm::vec3(0, 3, 0), 0.3f);
			trees.Add(p);
		}
	}

	//マークを配置
	{
		const size_t markCount = 10;
		marks.Reserve(markCount);
	}

	////オープニングスクリプトを実行
	//SceneStack::Instance().Push(std::make_shared<EventScene>("Res/OpeningScript.txt"));

	//パーティクル・システムのテスト用にエミッターを追加
	{
		//地蔵用パーティクル
		for (auto o : objects) {
			ParticleEmitterParameter ep;

			ep.id = particleID;
			ep.imagePath = "Res/Mist.tga";
			ep.tiles = glm::ivec2(2, 2);
			ep.position = o->position;
			//ep.position = glm::vec3(96.5f, 0, 95);
			ep.position.y = heightMap.Height(ep.position);
			ep.emissionsPerSecond = 20.0f;
			ep.dstFactor = GL_ONE;//加算合成
			ep.gravity = 0;
			ParticleParameter pp;
			pp.scale = glm::vec2(0.5f);
			pp.color = glm::vec4(0.5f, 0.0f, 0.5f, 1.0f);
			particleSystem.Add(ep, pp);
			++particleID;
		}
	}

	//{
	//	//エミッター3個目
	//	ParticleEmitterParameter ep;
	//	ep.imagePath = "Res/DiskParticle.tga";
	//	ep.position = glm::vec3(90, 0, 90);
	//	ep.position.y = heightMap.Height(ep.position);
	//	ep.angle = glm::radians(60.0f);
	//	ParticleParameter pp;
	//	pp.lifetime = 2;
	//	pp.scale = glm::vec2(0.5f);
	//	pp.velocity = glm::vec3(0, 1, 0);
	//	pp.color = glm::vec4(0.1f, 0.9f, 0.3f, 1.0f);
	//	particleSystem.Add(ep, pp);
	//}
	return true;
}


/*
プレイヤーの入力を処理する
*/
void MainGameScene::ProcessInput()
{
	GLFWEW::Window& window = GLFWEW::Window::Instance();

	//プレイヤー操作
	player->ProcessInput();

	//if (!flag) {

	//	//GLFWEW::Window& window = GLFWEW::Window::Instance();
	//	/*if (window.GetGamePad().buttonDown& GamePad::START) {
	//		flag = true;
	//		SceneStack::Instance().Push(std::make_shared<StatusScene>());
	//	}*/
	//	if (timer <= 0 && (window.GetGamePad().buttonDown & GamePad::START)) {
	//		/*Audio::Engine::Instance().Prepare("Res/Audio/Start.wav")->Play();
	//		timer = 1.0f;*/
	//		if (window.GetGamePad().buttonDown& GamePad::START) {
	//			bgm->Stop();
	//			SceneStack::Instance().Replace(std::make_shared<GameOverScene>());
	//		}
	//	}
	//	else {
	//		//GLFWEW::Window& window = GLFWEW::Window::Instance();
	//		if (window.GetGamePad().buttonDown& GamePad::START) {
	//			bgm->Stop();
	//			SceneStack::Instance().Replace(std::make_shared<GameOverScene>());
	//		}
	//	}
	//}

}
/*
シーンを更新する

@param deltaTime  前回の更新からの経過時間(秒)
*/
void MainGameScene::Update(float deltaTime)
{
	//カメラの状態を更新
	{
		camera.target = player->position;
		camera.position = camera.target + glm::vec3(0, 8, 13);
	}
	player->Update(deltaTime);
	enemies.Update(deltaTime);
	trees.Update(deltaTime);
	objects.Update(deltaTime);
	lights.Update(deltaTime);
	//marks.Update(deltaTime);

	DetectCollision(player, enemies);
	DetectCollision(player, trees);
	DetectCollision(player, objects);
	DetectCollision(enemies, enemies);
	DetectCollision(enemies, trees);
	DetectCollision(enemies, objects);

	//プレイヤーの攻撃判定
	ActorPtr PlayerAttackCollision = player->GetAttackCollision();
	if (PlayerAttackCollision) {
		bool hit = false;
		DetectCollision(PlayerAttackCollision, enemies,
			[this, &hit](const ActorPtr& a, const ActorPtr& b, const glm::vec3& p) {
			EnemyActorPtr bb = std::static_pointer_cast<EnemyActor>(b);
			bb->health -= a->health;
			if (bb->health <= 0) {
				bb->colLocal = Collision::Shape{};
				hitEffect = true;
				bb->health = 1;

				bb->Dead();
				Audio::Engine::Instance().Prepare("Res/Audio/EnemyDead.wav")->Play();
				bb->GetMesh()->Play("Down", false);//敵の死亡時のアニメーション
			}
			else {
				hitEffect = true;
				Audio::Engine::Instance().Prepare("Res/Audio/PlayerAttack.wav")->Play();
				bb->GetMesh()->Play("Hit", false);//敵がダメージを受けた際のアニメーション
				bb->Damage();
			}
			hit = true;
		}
		);
		if (hit) {
			PlayerAttackCollision->health = 0;
		}
		hitEffect = false;
	}
	//死亡アニメーションの終わった敵を消す
	for (ActorPtr e : enemies) {
		SkeletalMeshActorPtr enemy = std::static_pointer_cast<SkeletalMeshActor>(e);
		Mesh::SkeletalMeshPtr mesh = enemy->GetMesh();
		if (mesh->IsFinished()) {
			if (mesh->GetAnimation() == "Down") {
				enemy->health = 0;
			}
			else {
				mesh->Play("Wait");
			}
		}
	}

	//エネミーの攻撃判定
	for (ActorPtr e : enemies) {
		EnemyActorPtr enemy = std::static_pointer_cast<EnemyActor>(e);
		ActorPtr EnemyAttackCollision = enemy->GetAttackCollision();

		if (EnemyAttackCollision) {
			bool hit = false;
			DetectCollision(EnemyAttackCollision, player,
				[this, &hit](const ActorPtr& a, const ActorPtr& b, const glm::vec3& p) {
				PlayerActorPtr bb = std::static_pointer_cast<PlayerActor>(b);
				bb->health -= a->health;
				if (bb->health <= 0) {
					bb->colLocal = Collision::Shape{};
					hitEffect = true;
					bb->Dead();
					bgm->Stop();
					Audio::Engine::Instance().Prepare("Res/Audio/PlayerDead.wav")->Play();
					bb->GetMesh()->Play("Down", false);//プレイヤーの死亡時のアニメーション
				}
				else {
					hitEffect = true;
					Audio::Engine::Instance().Prepare("Res/Audio/EnemyAttack.wav")->Play();
					bb->GetMesh()->Play("Hit", false);//プレイヤーがダメージを受けた際のアニメーション
					bb->Damage(); bool nowCps = false;
				}
				hit = true;
			}
			);
			if (hit) {
				EnemyAttackCollision->health = 0;
			}
			hitEffect = false;
		}
	}
	if (-2 > player->position.y)
	{
		PlayerActorPtr bb = std::static_pointer_cast<PlayerActor>(player);
		bb->health = 0;
		bb->Dead();
		bgm->Stop();
		Audio::Engine::Instance().Prepare("Res/Audio/PlayerDead.wav")->Play();
		bb->GetMesh()->Play("Down", false);//プレイヤーの死亡時のアニメーション
	}

	for (auto e : enemies) {
		if (hitEffect) {
			//エミッター2個目
			ParticleEmitterParameter ep;
			ep.imagePath = "Res/FireParticle.tga";
			ep.position = e->position;
			ep.position.y += 0.8f;
			ep.angle = glm::radians(90.0f);
			ep.emissionsPerSecond = 50.0f;	//秒あたりのパーティクル放出数
			ep.radius = 1.0f;				//放出方向を定義する円錐の半径
			ep.gravity = 0.0f;				//パーティクルにかかる重力
			ep.loop = false;
			ParticleParameter pp;
			pp.lifetime = 1;
			pp.scale = glm::vec2(1.0f);
			pp.velocity = e->rotation;
			pp.color = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
			particleSystem.Add(ep, pp);
		}
	}

	//ライトの更新
	glm::vec3 ambientColor(0.1f, 0.05f, 0.15f);
	lightBuffer.Update(lights, ambientColor);
	for (auto e : trees) {
		const std::vector<ActorPtr> neighborhood = lights.FindNearbyActors(e->position, 20);
		std::vector<int> pointLightIndex;
		std::vector<int> spotLightIndex;
		pointLightIndex.reserve(neighborhood.size());
		spotLightIndex.reserve(neighborhood.size());
		for (auto light : neighborhood) {
			if (PointLightActorPtr p = std::dynamic_pointer_cast<PointLightActor>(light)) {
				if (pointLightIndex.size() < 8) {
					pointLightIndex.push_back(p->index);
				}
			}
			else if (
				SpotLightActorPtr p = std::dynamic_pointer_cast<SpotLightActor>(light)) {
				if (spotLightIndex.size() < 8) {
					spotLightIndex.push_back(p->index);
				}
			}
		}
		StaticMeshActorPtr p = std::static_pointer_cast<StaticMeshActor>(e);
		p->SetPointLightList(pointLightIndex);
		p->SetSpotLightList(spotLightIndex);
	}

	particleSystem.Update(deltaTime);

	//敵を全滅させたら目的達成フラグをtrueにする
	if (jizoId >= 0) {
		if (enemies.Empty()) {
			particleSystem.Remove(particleSystem.Find(jizoId));
			achivements[jizoId] = true;
			jizoId = -1;
			--jizoCount;
		}
	}


	if (achivements[0] &&
		achivements[1] &&
		achivements[2] &&
		achivements[3] == true) {
		bgm->Stop();
		SceneStack::Instance().Replace(std::make_shared<ClearScene>());
		return;
	}
	if (player->dead == true) {
		if (player->GetMesh()->IsFinished()) {
			bgm->Stop();
			SceneStack::Instance().Replace(std::make_shared<GameOverScene>());
			return;
		}
	}

	player->UpdateDrawData(deltaTime);
	enemies.UpdateDrawData(deltaTime);
	trees.UpdateDrawData(deltaTime);
	objects.UpdateDrawData(deltaTime);
	//marks.UpdateDrawData(deltaTime);

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

	std::wstringstream ss1, ss2, ss3, ss4;
	ss1 << L"未開放地蔵:" << jizoCount;
	fontRenderer.AddString(glm::vec2(-w * 0.5f + 20, h * 0.5f - lineHeight), ss1.str().c_str());
	ss2 << L"HP:" << player->health;
	fontRenderer.AddString(glm::vec2(w * 0.4f + 20, h * 0.5f - lineHeight), ss2.str().c_str());
	/*if (enep) {
		ss3 << L"デバッグ用:" << enep->A;
		fontRenderer.AddString(glm::vec2(w * 0.0f + 20, h * 0.5f - lineHeight), ss3.str().c_str());
	}
	if (enep) {
		ss4 << L"デバッグ用:" << enep->B;
		fontRenderer.AddString(glm::vec2(-w * 0.25f + 20, h * 0.5f - lineHeight), ss4.str().c_str());
	}*/

	//fontRenderer.AddString(glm::vec2(-128, 0), L"アクションゲーム");
	fontRenderer.EndUpdate();

	//シーン切り替え待ち
	if (timer > 0) {
		timer -= deltaTime;
		if (timer <= 0) {
			bgm->Stop();
			SceneStack::Instance().Replace(std::make_shared<StatusScene>());
			return;
		}
	}
	textWindow.Update(deltaTime);
}

/*
シーンを描画する
*/
void MainGameScene::Render()
{
	//影用FBOに描画
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fboShadow->GetFramebuffer());
		auto tex = fboShadow->GetDepthTexture();
		glViewport(0, 0, tex->Width(), tex->Height());
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		//ディレクショナル・ライトの向きから影用のビュー行列を作成
		//視点は、カメラの注視点からライト方向に100m移動した位置に設定する
		glm::vec3 direction(0, -1, 0);
		for (auto e : lights) {
			if (auto p = std::dynamic_pointer_cast<DirectionalLightActor>(e)) {
				direction = p->direction;
				break;
			}
		}
		const glm::vec3 position = camera.target - direction * 100.0f;
		const glm::mat4 matView = glm::lookAt(position, camera.target, glm::vec3(0, 1, 0));

		//平行投影によるプロジェクション行列を作成
		const float width = 100; //描画範囲の幅
		const float height = 100; //描画範囲の高さ
		const float near = 10.0f; //描画範囲の手前側の境界
		const float far = 200.0f; //描画範囲の奥側の境界
		const glm::mat4 matProj =
			glm::ortho<float>(-width / 2, width / 2, -height / 2, height / 2, near, far);

		//ビュー・プロジェクション行列を設定してメッシュを描画
		meshBuffer.SetShadowViewProjectionMatrix(matProj*matView);
		RenderMesh(Mesh::DrawType::shadow);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, fboMain->GetFramebuffer());
	const auto texMain = fboMain->GetColorTexture();
	glViewport(0, 0, texMain->Width(), texMain->Height());

	const GLFWEW::Window& window = GLFWEW::Window::Instance();
	const glm::vec2 screenSize(window.Width(), window.Height());
	spriteRenderer.Draw(screenSize);

	glEnable(GL_DEPTH_TEST);

	lightBuffer.Upload();
	lightBuffer.Bind();

	//FBOに描画
	glBindFramebuffer(GL_FRAMEBUFFER, fboMain->GetFramebuffer());
	glClearColor(0.5f, 0.6f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	const glm::mat4 matView = glm::lookAt(camera.position, camera.target, camera.up);
	const float aspectRatio =
		static_cast<float>(window.Width()) / static_cast<float>(window.Height());
	const glm::mat4 matProj =
		glm::perspective(camera.fov*0.5f, aspectRatio, camera.near, camera.far);

	meshBuffer.SetViewProjectionMatrix(matProj*matView);
	meshBuffer.SetCameraPosition(camera.position);
	meshBuffer.SetTime(window.Time());
	meshBuffer.BindShadowTexture(fboShadow->GetDepthTexture());

	RenderMesh(Mesh::DrawType::color);
	particleSystem.Draw(matProj, matView);

	meshBuffer.UnbindShadowTexture();

	//被写界深度エフェクト
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fboDepthOfField->GetFramebuffer());
		const auto tex = fboDepthOfField->GetColorTexture();
		glViewport(0, 0, tex->Width(), tex->Height());

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);

		camera.Update(matView);

		Mesh::FilePtr mesh = meshBuffer.GetFile("RenderTarget");
		Shader::ProgramPtr prog = mesh->materials[0].program;
		prog->Use();
		prog->SetViewInfo(static_cast<float>(window.Width()),
			static_cast<float>(window.Height()), camera.near, camera.far);
		prog->SetCameraInfo(camera.focalPlane, camera.focalLength,
			camera.aperture, camera.sensorSize);
		Mesh::Draw(mesh, glm::mat4(1));
	}

	//ブルーム・エフェクト
	{
		//明るい部分を取り出す
		{
			auto tex = fboBloom[0][0]->GetColorTexture();
			glBindFramebuffer(GL_FRAMEBUFFER, fboBloom[0][0]->GetFramebuffer());
			glViewport(0, 0, tex->Width(), tex->Height());
			glClear(GL_COLOR_BUFFER_BIT);
			Mesh::FilePtr mesh = meshBuffer.GetFile("BrightPassFilter");
			mesh->materials[0].texture[0] = fboDepthOfField->GetColorTexture();
			Mesh::Draw(mesh, glm::mat4(1));
		}
		//縮小コピー
		Mesh::FilePtr simpleMesh = meshBuffer.GetFile("Simple");
		for (int i = 0; i < sizeof(fboBloom) / sizeof(fboBloom[0]) - 1; ++i) {
			auto tex = fboBloom[i + 1][0]->GetColorTexture();
			glBindFramebuffer(GL_FRAMEBUFFER, fboBloom[i + 1][0]->GetFramebuffer());
			glViewport(0, 0, tex->Width(), tex->Height());
			glClear(GL_COLOR_BUFFER_BIT);
			simpleMesh->materials[0].texture[0] = fboBloom[i][0]->GetColorTexture();
			Mesh::Draw(simpleMesh, glm::mat4(1));
		}
		//ガウスぼかし
		Mesh::FilePtr blurMesh = meshBuffer.GetFile("NormalBlur");
		Shader::ProgramPtr progBlur = blurMesh->materials[0].program;
		for (int i = sizeof(fboBloom) / sizeof(fboBloom[0]) - 1; i >= 0; --i) {
			auto tex = fboBloom[i][0]->GetColorTexture();
			glBindFramebuffer(GL_FRAMEBUFFER, fboBloom[i][1]->GetFramebuffer());
			glViewport(0, 0, tex->Width(), tex->Height());
			glClear(GL_COLOR_BUFFER_BIT);
			progBlur->Use();
			progBlur->SetBlurDirection(1.0f / static_cast<float>(tex->Width()), 0.0f);
			blurMesh->materials[0].texture[0] = fboBloom[i][0]->GetColorTexture();
			Mesh::Draw(blurMesh, glm::mat4(1));

			glBindFramebuffer(GL_FRAMEBUFFER, fboBloom[i][0]->GetFramebuffer());
			glClear(GL_COLOR_BUFFER_BIT);
			progBlur->Use();
			progBlur->SetBlurDirection(0.0f, 1.0f / static_cast<float>(tex->Height()));
			blurMesh->materials[0].texture[0] = fboBloom[i][1]->GetColorTexture();
			Mesh::Draw(blurMesh, glm::mat4(1));
		}
		//拡大&加算合成
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		for (int i = sizeof(fboBloom) / sizeof(fboBloom[0]) - 1; i > 0; --i) {
			auto tex = fboBloom[i - 1][0]->GetColorTexture();
			glBindFramebuffer(GL_FRAMEBUFFER, fboBloom[i - 1][0]->GetFramebuffer());
			glViewport(0, 0, tex->Width(), tex->Height());
			simpleMesh->materials[0].texture[0] = fboBloom[i][0]->GetColorTexture();
			Mesh::Draw(simpleMesh, glm::mat4(1));
		}
	}
	//全てをデフォルト・フレームバッファに合成描画
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window.Width(), window.Height());

		const glm::vec2 screenSize(window.Width(), window.Height());
		//spriteRenderer.Draw(screenSize);
		spriteRenderer.Draw(screenSize);

		//被写界深度エフェクト適用後の画像を描画
		glDisable(GL_BLEND);
		Mesh::FilePtr simpleMesh = meshBuffer.GetFile("Simple");
		simpleMesh->materials[0].texture[0] = fboDepthOfField->GetColorTexture();
		Mesh::Draw(simpleMesh, glm::mat4(1));

		//拡散光を描画
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		simpleMesh->materials[0].texture[0] = fboBloom[0][0]->GetColorTexture();
		Mesh::Draw(simpleMesh, glm::mat4(1));

		textWindow.Draw();
		fontRenderer.Draw(screenSize);
	}

	////デバッグのために、影用の深度テクスチャを表示する
	//{
	//	glDisable(GL_BLEND);
	//	Mesh::FilePtr simpleMesh = meshBuffer.GetFile("Simple");
	//	simpleMesh->materials[0].texture[0] = fboShadow->GetDepthTexture();
	//	glm::mat4 m = glm::scale(glm::translate(
	//		glm::mat4(1), glm::vec3(-0.45f, 0, 0)), glm::vec3(0.5f, 0.89f, 1));
	//	Mesh::Draw(simpleMesh, m);
	//}
}

/*
お地蔵様に触れたときの処理

@param id  お地蔵様の番号
@param pos お地蔵様の座標

@retval true  処理成功
@retval false すでに戦闘中なので処理しなかった
*/
bool MainGameScene::HandleJizoEffects(int id, const glm::vec3& pos)
{
	if (jizoId >= 0) {
		return false;
	}
	jizoId = id;
	const size_t oniCount = 1;//出現させる敵の数
	for (size_t i = 0; i < oniCount; i++)
	{
		glm::vec3 position(pos);
		position.x += std::uniform_real_distribution<float>(-10, 10)(rand);
		position.z += std::uniform_real_distribution<float>(-10, 10)(rand);
		position.y = heightMap.Height(position);

		glm::vec3 rotation(0);
		rotation.y = std::uniform_real_distribution<float>(0, 3.14f * 2.0f)(rand);
		enep = std::make_shared<EnemyActor>(
			&heightMap, meshBuffer, position, rotation);
		enep->colLocal = Collision::CreateCapsule(
			glm::vec3(0, 0.5f, 0), glm::vec3(0, 1, 0), 0.5f);

			//追いかけるターゲットを指定
		enep->SetTarget(player);
		enemies.Add(enep);
	}
	//const Mesh::FilePtr mesh = meshBuffer.GetFile("Res/exclamation.gltf");
	//const size_t markCount = oniCount;//鬼の数だけマークを用意する
	//for (size_t i = 0; i < markCount; i++)
	//{
	//	glm::vec3 position(pos);
	//	position.x = enep->position.x;
	//	position.z = enep->position.z;
	//	position.y = heightMap.Height(position)+3.0f;

	//	glm::vec3 rotation(0);
	//	StaticMeshActorPtr p = std::make_shared<StaticMeshActor>(
	//		mesh, "marks", 13, position, rotation);
	//	/*p->colLocal = Collision::CreateCapsule(
	//		glm::vec3(0, 0, 0), glm::vec3(0, 3, 0), 0.3f);*/
	//	marks.Add(p);
	//}


	return true;
}

/*
カメラのパラメータを更新する

@param matView 更新に使用するビュー行列
*/
void MainGameScene::Camera::Update(const glm::mat4& matView)
{
	const glm::vec4 pos = matView * glm::vec4(target, 1);
	focalPlane = pos.z * -1000.0f;

	const float imageDistance = sensorSize * 0.5f / glm::tan(fov * 0.5f);
	focalLength = 1.0f / ((1.0f / focalPlane) + (1.0f / imageDistance));
	aperture = focalLength / fNumber;
}

/*
メッシュを描画する
*/
void MainGameScene::RenderMesh(Mesh::DrawType drawType)
{
	/*glm::vec3 cubePos(100, 0, 100);
	cubePos.y = heightMap.Height(cubePos);
	const glm::mat4 matModel = glm::translate(glm::mat4(1), cubePos);
	Mesh::Draw(meshBuffer.GetFile("Cube"), matModel,drawType);*/
	Mesh::Draw(meshBuffer.GetFile("Terrain"), glm::mat4(1), drawType);

	player->Draw(drawType);
	enemies.Draw(drawType);
	trees.Draw(drawType);
	objects.Draw(drawType);
	//marks.Draw(drawType);

	glm::vec3 treePos(110, 0, 110);
	treePos.y = heightMap.Height(treePos);
	const glm::mat4 matTreeModel =
		glm::translate(glm::mat4(1), treePos) * glm::scale(glm::mat4(1), glm::vec3(3));
	//Mesh::Draw(meshBuffer.GetFile("Res/red_pine_tree.gltf"),  matTreeModel,drawType);

	if (drawType == Mesh::DrawType::color)
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	Mesh::Draw(meshBuffer.GetFile("Water"), glm::mat4(1), drawType);
}
