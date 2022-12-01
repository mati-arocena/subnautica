#include "VBO.h"

VBO::VBO()
{
	glGenBuffers(1, &id);
	this->lenght = 0;
	this->vertices = nullptr;
}

void VBO::load(std::vector<Vertex> vertices, size_t lenght)
{
	bind();
	this->vertices_v = vertices;
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices_v.size(), &vertices_v[0], GL_STATIC_DRAW);
}

void VBO::load(float* vertices, size_t lenght)
{
	bind();
	this->lenght = lenght;
	this->vertices = vertices;
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * lenght, this->vertices, GL_STATIC_DRAW);
}

void VBO::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VBO::destroy()
{
	if (vertices_v.size() > 0)
	{
		glDeleteBuffers(sizeof(Vertex) * vertices_v.size(), &id);
	}
	else
	{
		glDeleteBuffers(static_cast<GLsizei>(lenght * sizeof(float)), &id);
	}
}

VBO::~VBO()
{
	destroy();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

unsigned int* VBO::toEBO(const std::vector<unsigned int>& indices)
{
	unsigned int* eboBuffer = new unsigned int[indices.size()];
	for (size_t i = 0; i < indices.size(); ++i)
	{
		eboBuffer[i] = indices[i];
	}
	return eboBuffer;
}
