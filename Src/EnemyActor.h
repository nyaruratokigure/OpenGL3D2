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
�G�l�~�[�A�N�^�[
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

	/// �A�j���[�V�������
	enum class State {
		idle, ///<��~
		run,  ///<�ړ�
		jump, ///<�W�����v
		attack, ///< �U��
	};
	State state = State::idle; ///<���݂̃A�j���[�V�������
	bool isInAir = false;      ///<�󒆔���t���O
	ActorPtr boardingActor;    ///<����Ă���A�N�^�[
	float moveSpeed = 3.0f;    ///<�ړ����x
	ActorPtr attackCollision;  ///<�U������
	float attackTimer = 0;     ///<�U������
	
	
	const Terrain::HeightMap* heightMap = nullptr;
};
using EnemyActorPtr = std::shared_ptr<EnemyActor>;

#endif // ENEMYACTOR_H_INCLUDED