/*
@file Mark.cpp
*/
#include "Mark.h"

/*
コンストラクタ

@param m		表示するメッシュ
@param id       管理用ID
@param position 位置
@param scare    拡大率 
*/
Mark::Mark(const Mesh::FilePtr& m,int id, const glm::vec3& position, const glm::vec3& scale)
	:StaticMeshActor(m,name,health,position,rotation,scale)
{

}

//マークの表示をする
void Mark::Add(){
	const Mesh::FilePtr mesh = meshBuffer.GetFile("Res/exclamation.gltf");
	for (size_t i = 0; i < markCount; ++i) {
		p = std::make_shared<StaticMeshActor>(
			mesh, "marks", 13, position, rotation, scale);
		marks.Add(p);
	}
}

//マークを更新する
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