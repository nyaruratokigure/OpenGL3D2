/*
@file Mark.cpp
*/
#include "Mark.h"
#include "EnemyActor.h"

/*
�R���X�g���N�^

@param m		�\�����郁�b�V��
@param position �ʒu
@param scare    �g�嗦 
*/
Mark::Mark(const Mesh::FilePtr& m,int health, const glm::vec3& position, const glm::vec3& scale)
	:StaticMeshActor(m,name,health,position,rotation,scale)
{

}

//�}�[�N���X�V����
void Mark::Update(float deltaTime) {
	timer -= deltaTime;
	if (timer <= 0) {

		position = glm::vec3(0);

	}

}

//int Mark::Brunch(){
//
//}