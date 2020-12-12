/*
@file Mark.h
*/
#ifndef MARK_H_INCLUDED
#define MARK_H_INCLUDED
#include "Mesh.h"
#include "Actor.h"
#include "EnemyActor.h"
#include "glm/glm.hpp"
#include <memory>

class Mark : public StaticMeshActor
{
public:
	Mark(const Mesh::FilePtr& m, int id, const glm::vec3& pos,
		const glm::vec3&scale = glm::vec3(1));
	virtual ~Mark() = default;
	void Update(float deltaTime);

	
	

private:
	int id;
	glm::vec3 position = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);
	Mesh::Buffer meshBuffer;
	EnemyActorPtr enemy;
	ActorList mark;
};

using MarkPtr = std::shared_ptr<Mark>;

#endif MARK_H_INCLUDED