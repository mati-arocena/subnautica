#include "VBO.h"

VBO::VBO()
{
	glGenBuffers(1, &id);
	this->lenght = 0;
	this->vertices = nullptr;
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
	glDeleteBuffers(lenght * sizeof(float), &id);
}

VBO::~VBO()
{
	destroy();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
