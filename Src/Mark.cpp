/*
@file Mark.cpp
*/
#include "Mark.h"

/*
コンストラクタ

@param id       管理用ID
@param position 位置
@param scare    拡大率 
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