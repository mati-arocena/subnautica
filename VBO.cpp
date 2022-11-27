#include "VBO.h"

VBO::VBO()
{
	glGenBuffers(1, &id);
}

void VBO::load(float* verticesLOD0, size_t lenght)
{
	bind();
	this->lenght = lenght;
	this->verticesLOD0 = verticesLOD0;
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * lenght, this->verticesLOD0, GL_STATIC_DRAW);
}

void VBO::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VBO::destroy()
{
	glDeleteBuffers(lenght * sizeof(float), &id);
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
