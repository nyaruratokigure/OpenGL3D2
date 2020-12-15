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
	void Add();
	void Update(float deltaTime);

	
	

private:
	int id;
	glm::vec3 position = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);

	int markCount = 3;
	float timer = 2.0f;

	StaticMeshActorPtr p;
	Mesh::Buffer meshBuffer;
	ActorList marks;
};

using MarkPtr = std::shared_ptr<Mark>;

#endif MARK_H_INCLUDED