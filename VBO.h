#pragma once
#include <gl/glew.h>
#include <vector>

class VBO
{
public:
	VBO();
	void load(float* vertices, size_t lenght);
	void bind();
	void destroy();
	~VBO();

	static unsigned int* toEBO(const std::vector<unsigned int>& indices); 
private:
	unsigned int id;
	size_t lenght;
	float* vertices;
	void setVertexAttribute(int id, int stride);
};

