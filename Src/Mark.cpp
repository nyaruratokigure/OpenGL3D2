/*
@file Mark.cpp
*/
#include "Mark.h"

/*
�R���X�g���N�^

@param m		�\�����郁�b�V��
@param id       �Ǘ��pID
@param position �ʒu
@param scare    �g�嗦 
*/
Mark::Mark(const Mesh::FilePtr& m,int id, const glm::vec3& position, const glm::vec3& scale)
	:StaticMeshActor(m,name,health,position,rotation,scale)
{

}

//�}�[�N�̕\��������
void Mark::Add(){
	const Mesh::FilePtr mesh = meshBuffer.GetFile("Res/exclamation.gltf");
	for (size_t i = 0; i < markCount; ++i) {
		p = std::make_shared<StaticMeshActor>(
			mesh, "marks", 13, position, rotation, scale);
		marks.Add(p);
	}
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