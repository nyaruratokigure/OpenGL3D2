/*
@file EnemyActor.h
*/
#ifndef ENEMYACTOR_H_INCLUDED
#define ENEMYACTOR_H_INCLUDED
#define _USE_MATH_DEFINES
#include "GLFWEW.h"
#include "SkeletalMeshActor.h"
#include "Terrain.h"
#include <memory>
#include <math.h>

/*
エネミーアクター
*/
class EnemyActor : public SkeletalMeshActor
{
public:
	EnemyActor(const Terrain::HeightMap* hm, const Mesh::Buffer& buffer,
		const glm::vec3& pos, const glm::vec3&rot = glm::vec3(0));
	virtual ~EnemyActor() = default;

	virtual void Update(float) override;
	virtual void OnHit(const ActorPtr&, const glm::vec3&);
	void SetBoardingActor(ActorPtr p);
	const ActorPtr& GetAttackCollision() const { return attackCollision; }
	bool dead=false;
	void Dead() {
		dead = true;
	}
	void Damage();
	float actionTimer = 0.0f;  ///<行動制御用のタイマー
	int nowAction = 0;         ///<選択中の行動.0未発見、1ニュートラル、2移動、3攻撃、4ダメージ、5フェイント
	int feintD = 0;          ///<フェイントの方向
	
private:
	float PlayerDist();
	void Inactive();///<非アクティブ、プレイヤーを発見する前の状態
	void Move();///<playerの座標に向けて移動する
	void Attack();///<攻撃する際にMoveから移行する
	void Feint();///<フェイント、左右と後退りの3パターン


	/// アニメーション状態
	enum class State {
		idle, ///<停止
		inactive,///非アクティブ時
		run,  ///<移動
		attack, ///< 攻撃
		damage,///<ダメージを受けた際
		feint,///<フェイント
		dead,///<死亡時
	};
	State state = State::idle; ///<現在のアニメーション状態
	bool isInAir = false;      ///<空中判定フラグ
	bool nowAttack = false;	   ///<攻撃中かどうか
	bool onlyOnce = false;
	float moveSpeed = 3.0f;    ///<移動速度
	float feintSpped = 2.0f;   ///<フェイント時の移動速度
	float attackTimer = 0.0f;  ///<攻撃時間	
	
	
	ActorPtr attackCollision;  ///<攻撃判定
	ActorPtr boardingActor;    ///<乗っているアクター
	

	//int probability = 0;	   ///<確率
	glm::vec3 targetPos;
	glm::vec3 move;
	glm::vec3 direction;


	
	
	const Terrain::HeightMap* heightMap = nullptr;
};
using EnemyActorPtr = std::shared_ptr<EnemyActor>;

#endif // ENEMYACTOR_H_INCLUDED