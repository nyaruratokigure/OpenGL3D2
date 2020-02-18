/*
@file Particle.h
*/
#ifndef PARTICLE_H_INCLUDED
#define PARTICLE_H_INCLUDED
#include <GL/glew.h>
#include "Texture.h"
#include "BufferObject.h"
#include "Shader.h"
#include "Sprite.h"
#include <glm/glm.hpp>
#include <list>
#include <memory>

//先行宣言
class Particle;
class ParticleEmitter;
using ParticleEmitterPtr = std::shared_ptr<ParticleEmitter>;
class ParticleSystem;

/*
パーティクルのパラメーター
*/
struct ParticleParameter
{
	float lifetime = 1;							//生存時間
	glm::vec3 velocity = glm::vec3(0, 2, 0);	//速度
	glm::vec3 acceleration = glm::vec3(0);		//加速度
	glm::vec2 scale = glm::vec2(1);				//大きさ
	float rotation = 0;							//回転
	glm::vec4 color = glm::vec4(1);				//色と不透明度
};

/*
パーティクル・エミッターのパラメーター
*/
struct ParticleEmitterParameter
{
	int id = 0;							//エミッターを制御するためのID
	glm::vec3 position = glm::vec3(0);	//エミッターの位置
	glm::vec3 rotation = glm::vec3(0);	//エミッターの向き
	float duration = 1.0f;				//パーティクルを放出する秒数
	bool loop = true;					//ループ再生するならtrue、停止するならfalse
	float emissionsPerSecond = 5.0f;	//秒あたりのパーティクル放出数
	float angle = glm::radians(15.0f);	//放出方向を定義する円錐の角度
	float radius = 0.5f;				//放出方向を定義する円錐の半径
	float gravity = 9.8f;				//パーティクルにかかる重力
	std::string imagePath;				//テクスチャ・ファイル名
	
	glm::ivec2 tiles = glm::ivec2(1);	//テクスチャの縦と横の分割数
	GLenum srcFactor = GL_SRC_ALPHA;
	GLenum dstFactor = GL_ONE_MINUS_SRC_ALPHA;
};

/*
パーティクル
*/
class Particle {
public:
	friend ParticleSystem;
	friend ParticleEmitter;

	Particle(const ParticleParameter& pp, const glm::vec3& pos, const Rect& r);
	virtual ~Particle() = default;

	void Update(float deltatime);
	bool IsDead() const { return lifetime <= 0; }

private:
	Rect rect = { glm::vec2(0), glm::vec2(0) }; //テクスチャ表示範囲
	glm::vec3 position = glm::vec3(0);		//位置
	float lifetime = 1;						//生存期間
	glm::vec3 velocity = glm::vec3(0, 2, 0);//速度
	glm::vec3 acceleration = glm::vec3(0);	//加速度
	glm::vec2 scale = glm::vec2(1);			//大きさ
	float rotation = 0;						//回転
	glm::vec4 color = glm::vec4(1);			//色と不透明度
};

/*
パーティクル放出クラス
*/
class ParticleEmitter
{
public:
	friend ParticleSystem;

	ParticleEmitter(const ParticleEmitterParameter& ep, const ParticleParameter& pp);
	~ParticleEmitter() = default;

	void Update(float deltatime);
	void Draw();

	//座標の設定・取得
	void Position(const glm::vec3& p) { ep.position = p; }
	const glm::vec3& Position()const { return ep.position; }

	bool IsDead() const { return !ep.loop&&timer >= ep.duration&&particles.empty(); }

private:
	void AddParticle();
	ParticleEmitterParameter ep; //エミッターのパラメーター
	ParticleParameter pp;        //パーティクルのパラメーター
	
	Texture::Image2DPtr texture; //パーティクル用テクスチャ
	float interval = 0;          //パーティクルの発生間隔(秒)
	float timer = 0;             //経過時間(秒)
	float emissionTimer = 0;     //パーティクル発生タイマー(秒)
	
	size_t count = 0;      //描画するインデックス数
	size_t baseVertex = 0; //描画の基準となる頂点のオフセット
	
	std::list<Particle>  particles; //パーティクルリスト
};

/*
パーティクル管理クラス
*/
class ParticleSystem 
{
public:
	ParticleSystem() = default;
	~ParticleSystem() = default;

	bool Init(size_t maxParticlecount);
	ParticleEmitterPtr Add(
		const ParticleEmitterParameter& ep, const ParticleParameter& pp);
	ParticleEmitterPtr Find(int id) const;
	void Remove(const ParticleEmitterPtr&);
	void Clear();
	void Update(float deltatime);
	void Draw(const glm::mat4& matProj, const glm::mat4& matView);

private:
	BufferObject vbo;
	BufferObject ibo;
	VertexArrayObject vao;
	Shader::ProgramPtr program;
	std::list<ParticleEmitterPtr> emitters;
};


#endif //PARTICLE_H_INCLUDED
