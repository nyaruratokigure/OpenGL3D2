/*
@file Mark.cpp
*/
#include "Mark.h"

/*
�R���X�g���N�^

@param m		�\�����郁�b�V��
@param position �ʒu
@param scare    �g�嗦 
*/
Mark::Mark(const Mesh::FilePtr& m, const glm::vec3& position, const glm::vec3& scale)
	:StaticMeshActor(m,name,health,position,rotation,scale)
{

}

//�}�[�N���X�V����
void Mark::Update(float deltaTime) {
	if (timer >= 0) {
		timer -= deltaTime;
	}
	else {
		for (size_t i = 0; i < markCount; ++i) {
			marks.Remove(p);
		}
	}
	position.y += 2.0f;
	
}