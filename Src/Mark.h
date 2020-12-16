/*
@file Mark.h
*/
#ifndef MARK_H_INCLUDED
#define MARK_H_INCLUDED
#include "Mesh.h"
#include "Actor.h"
#include "glm/glm.hpp"
#include <memory>

class Mark : public StaticMeshActor
{
public:
	Mark(const Mesh::FilePtr& m, const glm::vec3& pos,
		const glm::vec3&scale = glm::vec3(1));
	virtual ~Mark() = default;
	void Update(float deltaTime);

	
	

private:
	int markCount = 3;
	float timer = 2.0f;

	StaticMeshActorPtr p;
	Mesh::Buffer meshBuffer;
	ActorList marks;
};

using MarkPtr = std::shared_ptr<Mark>;

#endif MARK_H_INCLUDED