#pragma once
#include <gl/glew.h>

class VBO
{
public:
	VBO();
	void load(float* verticesLOD0, size_t lenght);
	void bind();
	void destroy();
	~VBO();

private:
	unsigned int id;
	size_t lenght;
	float* verticesLOD0;

};