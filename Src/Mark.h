/*
@file Mark.h
*/
#ifndef MARK_H_INCLUDED
#define MARK_H_INCLUDED
#include "Mesh.h"
#include "glm/glm.hpp"
#include <memory>

class Mark
{
public:
	Mark(int id, const glm::vec3& pos,
		const glm::vec3&scale = glm::vec3(1));
	virtual ~Mark() = default;

	virtual void Update(float);
	virtual void UpdateDrawData(float);
	virtual void Draw(Mesh::DrawType drawType);

public:
	int id;
	glm::vec3 position = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);
};

using MarkPtr = std::shared_ptr<Mark>;

#endif MARK_H_INCLUDED