#include "VBO.h"

VBO::VBO()
{
	glGenBuffers(1, &id);
	this->lenght = 0;
	this->verticesLOD0_f = nullptr;
}

void VBO::load(std::vector<Vertex> verticesLOD0, size_t lenght)
{
	bind();
	this->verticesLOD0_v = verticesLOD0;
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verticesLOD0_v.size(), &verticesLOD0_v[0], GL_STATIC_DRAW);
}

void VBO::load(float* verticesLOD0, size_t lenght) 
{
	bind();
	this->lenght = lenght;
	this->verticesLOD0_f = verticesLOD0;
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * lenght, this->verticesLOD0_f, GL_STATIC_DRAW);
}

void VBO::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VBO::destroy()
{
	glDeleteBuffers(sizeof(Vertex) * verticesLOD0_v.size(), &id);
}

VBO::~VBO()
{
	destroy();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

unsigned int* VBO::toEBO(std::vector<unsigned int> indices)
{
	unsigned int* eboBuffer = new unsigned int[indices.size()];
	for (size_t i = 0; i < indices.size(); ++i)
	{
		eboBuffer[i] = indices[i];
	}
	return eboBuffer;
}
