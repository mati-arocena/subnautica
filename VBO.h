#pragma once
#include <gl/glew.h>
#include <vector>

class VBO
{
public:
	VBO();
	void load(float* verticesLOD0, size_t lenght);
	void bind();
	void destroy();
	~VBO();

	static unsigned int* toEBO(std::vector<unsigned int> indices); 

private:
	unsigned int id;
	size_t lenght;
	float* verticesLOD0;

};