#include "Mesh.h"
#include <GLFW/glfw3.h>

static int numElementsInVBO = 14;

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material *material, glm::mat4 modelMat)
{
	this->vertices = vertices;
	this->indices = indices;
	this->material = material;
	clipPlane = glm::vec4{ 0.f, 0.f, 0.f, 0.f };
	this->model = modelMat;

	glGenVertexArrays(1, &vao);
	vbo = new VBO();
	glGenBuffers(1, &ebo);
		
	// 1. bind Vertex Array Object
	glBindVertexArray(vao);
	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	vbo->load(Vertex::toVBO(vertices), vertices.size() * numElementsInVBO); // Vertices and qty
	// 3. copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), ToEBO(), GL_STATIC_DRAW);
	// 4. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, numElementsInVBO * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, numElementsInVBO * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, numElementsInVBO * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, numElementsInVBO * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, numElementsInVBO * sizeof(float), (void*)(11 * sizeof(float)));
	glEnableVertexAttribArray(4);

	
}

void Mesh::render()
{
	material->use();

	std::shared_ptr<Shader> shader = material->getShader();
	shader->setFloat("time", glfwGetTime());
	//esto es por ahora 
	shader->setFloat("clippingPlane", clipPlane.x, clipPlane.y, clipPlane.z, clipPlane.w);

	shader->setMat4("model", model);
	glBindVertexArray(vao);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // cantidad de indices
	glBindVertexArray(0);
}

void Mesh::setupMesh()
{
}

unsigned int* Mesh::ToEBO()
{
	unsigned int* eboBuffer = new unsigned int[indices.size()];
	for (size_t i = 0; i < indices.size(); ++i)
	{
		eboBuffer[i] = indices[i];
	}
	return eboBuffer;
}

float* Vertex::toVBO(std::vector<Vertex> vertices)
{
	float* vbo = new float[vertices.size() * numElementsInVBO];
	
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		vbo[i * numElementsInVBO     ] = vertices[i].Position.x;
		vbo[i * numElementsInVBO +  1] = vertices[i].Position.y;
		vbo[i * numElementsInVBO +  2] = vertices[i].Position.z;
		vbo[i * numElementsInVBO +  3] = vertices[i].Normal.x;
		vbo[i * numElementsInVBO +  4] = vertices[i].Normal.y;
		vbo[i * numElementsInVBO +  5] = vertices[i].Normal.z;
		vbo[i * numElementsInVBO +  6] = vertices[i].TexCoords.x;
		vbo[i * numElementsInVBO +  7] = vertices[i].TexCoords.y;
		vbo[i * numElementsInVBO +  8] = vertices[i].Tangent.x;
		vbo[i * numElementsInVBO +  9] = vertices[i].Tangent.y;
		vbo[i * numElementsInVBO + 10] = vertices[i].Tangent.z;
		vbo[i * numElementsInVBO + 11] = vertices[i].Bitangent.x;
		vbo[i * numElementsInVBO + 12] = vertices[i].Bitangent.y;
		vbo[i * numElementsInVBO + 13] = vertices[i].Bitangent.z;

	}

	return vbo;
}

void Mesh::setClipPlane(glm::vec4 plane)
{
	this->clipPlane = plane;
}

glm::vec4 Mesh::getClipPlane()
{
	return this->clipPlane;
}

void Mesh::render_withShader(std::shared_ptr<Shader> shader)
{
	shader->use();
	shader->setFloat("clippingPlane", clipPlane.x, clipPlane.y, clipPlane.z, clipPlane.w);

	shader->setMat4("model", model);

	glBindVertexArray(vao);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0); // cantidad de indices
	glBindVertexArray(0);


}
