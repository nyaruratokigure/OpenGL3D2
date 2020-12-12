/*
@file Mark.cpp
*/
#include "Mark.h"

/*
�R���X�g���N�^

@param id       �Ǘ��pID
@param position �ʒu
@param scare    �g�嗦 
*/
Mark::Mark(const Mesh::FilePtr& m,int id, const glm::vec3& position, const glm::vec3& scale)
	:StaticMeshActor(m,name,health,position,rotation,scale)
{
	const Mesh::FilePtr mesh = meshBuffer.GetFile("Res/exclamation.gltf");
	StaticMeshActorPtr p = std::make_shared<StaticMeshActor>(
		mesh, "marks", 10, position, scale);

}

void Mark::Update(float deltaTime) {
	position = enemy->position;
	position.y += 2.0f;
}