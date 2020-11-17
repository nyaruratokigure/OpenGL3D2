/*
@file PlayerActor.h
*/
#ifndef PLAYERACTOR_H_INCLUDED
#define PLAYERACTOR_H_INCLUDED
#include "GLFWEW.h"
#include "SkeletalMeshActor.h"
#include "Terrain.h"
#include <memory>

/*
プレイヤーアクター
*/
class PlayerActor : public SkeletalMeshActor
{
public:
	PlayerActor(const Terrain::HeightMap* hm, const Mesh::Buffer& buffer,
		const glm::vec3& pos, const glm::vec3&rot = glm::vec3(0));
	virtual ~PlayerActor() = default;

	virtual void Update(float) override;
	virtual void OnHit(const ActorPtr&, const glm::vec3&);
	void Jump();
	void ProcessInput();
	void SetBoardingActor(ActorPtr);
	const ActorPtr& GetAttackCollision() const { return attackCollision; }
	void Damage();

	bool dead = false;
	void Dead() {
		dead = true;
	}

private:
	void CheckRun(const GamePad& gamepad);
	void CheckJump(const GamePad& gamepad);
	void CheckAttack(const GamePad& gamepad);

	/// アニメーション状態
	enum class State {
		idle, ///<停止
		run,  ///<移動
		jump, ///<ジャンプ
		attack, ///< 攻撃
		damage,///<ダメージ
	};
	State state = State::idle; ///<現在のアニメーション状態
	bool isInAir = false;      ///<空中判定フラグ
	ActorPtr boardingActor;    ///<乗っているアクター
	float moveSpeed = 5.0f;    ///<移動速度
	ActorPtr attackCollision;  ///<攻撃判定
	float attackTimer = 0;     ///<攻撃時間
	int attackCombo = 0;

	const Terrain::HeightMap* heightMap = nullptr;

};
using PlayerActorPtr = std::shared_ptr<PlayerActor>;

#endif // PLAYERACTOR_H_INCLUDED