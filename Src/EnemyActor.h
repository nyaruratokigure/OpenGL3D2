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
	int nowAction = 0;         ///<�I�𒆂̍s��.1�ʏ�A2�t�F�C���g�A3�������U��

private:
	void SRangeAttack();
	//void CheckJump();
	void Attack();
	void Feint();

	/// �A�j���[�V�������
	enum class State {
		idle, ///<��~
		run,  ///<�ړ�
		jump, ///<�W�����v
		attack, ///< �U��
	};
	State state = State::idle; ///<���݂̃A�j���[�V�������
	bool isInAir = false;      ///<�󒆔���t���O
	bool nowAttack = false;	   ///<�U�������ǂ���
	bool onlyOnce = false;
	float moveSpeed = 3.0f;    ///<�ړ����x
	float feintSpped = 3.0f;   ///<�t�F�C���g���̈ړ����x
	float attackTimer = 0.0f;  ///<�U������
	float actionTimer = 0.0f;  ///<�s������p�̃^�C�}�[
	ActorPtr attackCollision;  ///<�U������
	ActorPtr boardingActor;    ///<����Ă���A�N�^�[
	
	int probability = 0;	   ///<�m��
	int ver;
	int hor;
	glm::vec3 nowPosition;
	glm::vec3 thisPos;
	
	
	const Terrain::HeightMap* heightMap = nullptr;
};
using EnemyActorPtr = std::shared_ptr<EnemyActor>;

#endif // ENEMYACTOR_H_INCLUDED