/*
@file Mark.cpp
*/
#include "Mark.h"

/*
コンストラクタ

@param m		表示するメッシュ
@param position 位置
@param scare    拡大率 
*/
Mark::Mark(const Mesh::FilePtr& m, const glm::vec3& position, const glm::vec3& scale)
	:StaticMeshActor(m,name,health,position,rotation,scale)
{

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