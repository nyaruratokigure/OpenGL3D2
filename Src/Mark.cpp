/*
@file Mark.cpp
*/
#include "Mark.h"
#include "EnemyActor.h"

/*
コンストラクタ

@param m		表示するメッシュ
@param position 位置
@param scare    拡大率 
*/
Mark::Mark(const Mesh::FilePtr& m,int health, const glm::vec3& position, const glm::vec3& scale)
	:StaticMeshActor(m,name,health,position,rotation,scale)
{

}

//マークを更新する
void Mark::Update(float deltaTime) {
	timer -= deltaTime;
	if (timer <= 0) {

		position = glm::vec3(0);

	}

}

//int Mark::Brunch(){
//
//}