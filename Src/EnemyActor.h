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
	void Damage();
	float actionTimer = 0.0f;  ///<�s������p�̃^�C�}�[
	int nowAction = 0;         ///<�I�𒆂̍s��.0�������A1�j���[�g�����A2�ړ��A3�U���A4�_���[�W�A5�t�F�C���g
	int feintD = 0;          ///<�t�F�C���g�̕���
	
private:
	float PlayerDist();
	void Inactive();///<��A�N�e�B�u�A�v���C���[�𔭌�����O�̏��
	void Move();///<player�̍��W�Ɍ����Ĉړ�����
	void Attack();///<�U������ۂ�Move����ڍs����
	void Feint();///<�t�F�C���g�A���E�ƌ�ނ��3�p�^�[��


	/// �A�j���[�V�������
	enum class State {
		idle, ///<��~
		inactive,///��A�N�e�B�u��
		run,  ///<�ړ�
		attack, ///< �U��
		damage,///<�_���[�W���󂯂���
		feint,///<�t�F�C���g
		dead,///<���S��
	};
	State state = State::idle; ///<���݂̃A�j���[�V�������
	bool isInAir = false;      ///<�󒆔���t���O
	bool nowAttack = false;	   ///<�U�������ǂ���
	bool onlyOnce = false;
	float moveSpeed = 3.0f;    ///<�ړ����x
	float feintSpped = 2.0f;   ///<�t�F�C���g���̈ړ����x
	float attackTimer = 0.0f;  ///<�U������	
	
	
	ActorPtr attackCollision;  ///<�U������
	ActorPtr boardingActor;    ///<����Ă���A�N�^�[
	

	//int probability = 0;	   ///<�m��
	glm::vec3 targetPos;
	glm::vec3 move;
	glm::vec3 direction;


	
	
	const Terrain::HeightMap* heightMap = nullptr;
};
using EnemyActorPtr = std::shared_ptr<EnemyActor>;

#endif // ENEMYACTOR_H_INCLUDED