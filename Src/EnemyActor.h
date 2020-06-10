/*
@file EnemyActor.h
*/
#ifndef ENEMYACTOR_H_INCLUDED
#define ENEMYACTOR_H_INCLUDED
#include "GLFWEW.h"
#include "SkeletalMeshActor.h"
#include "Terrain.h"
#include <memory>

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
	

private:
	void CheckRun();
	void CheckJump();
	void CheckAttack();

	/// アニメーション状態
	enum class State {
		idle, ///<停止
		run,  ///<移動
		jump, ///<ジャンプ
		attack, ///< 攻撃
	};
	State state = State::idle; ///<現在のアニメーション状態
	bool isInAir = false;      ///<空中判定フラグ
	ActorPtr boardingActor;    ///<乗っているアクター
	float moveSpeed = 3.0f;    ///<移動速度
	ActorPtr attackCollision;  ///<攻撃判定
	float attackTimer = 0;     ///<攻撃時間
	
	
	const Terrain::HeightMap* heightMap = nullptr;
};
using EnemyActorPtr = std::shared_ptr<EnemyActor>;

#endif // ENEMYACTOR_H_INCLUDED