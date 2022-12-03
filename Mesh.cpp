#include "Mesh.h"
#include "GameInstance.h"
#include <GLFW/glfw3.h>
#include "Definitions.h"
#include "Animator.h"


int Vertex::numElementsInVBO = 14;

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
	Material* material, glm::mat4 modelMat, glm::vec3 min, glm::vec3 max)
	: vertices(vertices), indices(indices), material(material), minAABB(min), maxAABB(max)
{
	clipPlane = glm::vec4{ 0.f, 0.f, 0.f, 0.f };
	center = { (max + min) * 0.5f };
	extents = { max.x - center.x, max.y - center.y, max.z - center.z };

	vbo = new VBO();
	debugVBO = new VBO();

	setupMesh();

	debugShader = GameInstance::getInstance().getShader(FRUSTUM_SHADER);
	center = { (max + min) * 0.5f };
	extents = { max.x - center.x, max.y - center.y, max.z - center.z };

	Vertex nbl({ center.x - extents.x, center.y - extents.y, center.z + extents.z }, { 0.f, 1.f, 0.f }, { 0.f,1.f });
	Vertex nbr({ center.x + extents.x, center.y - extents.y, center.z + extents.z }, { 0.f, 1.f, 0.f }, { 0.f,1.f });
	Vertex ntl({ center.x - extents.x, center.y + extents.y, center.z + extents.z }, { 0.f, 1.f, 0.f }, { 0.f,1.f });
	Vertex ntr({ center.x + extents.x, center.y + extents.y, center.z + extents.z }, { 0.f, 1.f, 0.f }, { 0.f,1.f });
	Vertex fbl({ center.x - extents.x, center.y - extents.y, center.z - extents.z }, { 0.f, 1.f, 0.f }, { 0.f,1.f });
	Vertex fbr({ center.x + extents.x, center.y - extents.y, center.z - extents.z }, { 0.f, 1.f, 0.f }, { 0.f,1.f });
	Vertex ftl({ center.x - extents.x, center.y + extents.y, center.z - extents.z }, { 0.f, 1.f, 0.f }, { 0.f,1.f });
	Vertex ftr({ center.x + extents.x, center.y + extents.y, center.z - extents.z }, { 0.f, 1.f, 0.f }, { 0.f,1.f });

	std::vector<Vertex> verticesAABB = {
		nbl/*0*/, nbr/*1*/, ntl/*2*/, ntr/*3*/,
		fbl/*4*/, fbr/*5*/, ftl/*6*/, ftr/*7*/
	};
	std::vector<unsigned int> indicesAABB = {
		//near plane
		0, 1, 1, 3, 3, 2, 2, 0,
		//far plane
		4, 5, 5, 7, 7, 6, 6, 4,
		//bottom plane
		0, 4, 4, 5, 5, 1, 1, 0,
		//top plane
		2, 3, 3, 7, 7, 6, 6, 2
	};

	debugIndicesSize = static_cast<unsigned int>(indicesAABB.size());

	glGenVertexArrays(1, &debugVao);
	glGenBuffers(1, &debugEbo);

	glBindVertexArray(debugVao);	

	debugVBO->load(vertices, vertices.size() * Vertex::numElementsInVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, debugEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * debugIndicesSize, VBO::toEBO(indicesAABB), GL_STATIC_DRAW);
	
	Vertex::setVertexAttributes();

	decomposeModelMatrix(modelMat);
	computeModelMatrix();
	
}

void Mesh::render()
{
	material->use();

	std::shared_ptr<Shader> shader = material->getShader();
	shader->setFloat("time", static_cast<float>(glfwGetTime()));
	//esto es por ahora - Narrador: Esto quedará acá para siempre
	shader->setFloat("clippingPlane", clipPlane.x, clipPlane.y, clipPlane.z, clipPlane.w);
	shader->setMat4("model", model);

	
	shader->setBool("has_animation", this->animator != nullptr);
	if (this->animator != nullptr)
	{
		auto transforms = animator->getFinalBoneMatrices();
		for (int i = 0; i < transforms->size(); ++i)
			shader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms->at(i));
	}
	bind(GL_FILL);
	
	
}

void Mesh::setAnimator(std::shared_ptr<Animator> animator)
{
	this->animator = animator;
}

void Mesh::update(float delta)
{
	// At the end
	computeModelMatrix();
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &ebo);

	// 1. bind Vertex Array Object
	glBindVertexArray(vao);
	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	vbo->load(vertices, vertices.size() * Vertex::numElementsInVBO); // Vertices and qty
	// 3. copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), VBO::toEBO(indices), GL_STATIC_DRAW);


	// 4. then set the vertex attributes pointers
	// Vertex
	Vertex::setVertexAttributes();

}

void Vertex::setVertexAttributes()
{
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	// Normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(1);
	// TexCoords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	glEnableVertexAttribArray(2);
	// Tangent
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	glEnableVertexAttribArray(3);
	// Bitangent
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
	glEnableVertexAttribArray(4);
	// Bone ids
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
	// Bone weights
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
}

void Mesh::bind(GLenum polygonMode)
{
	glBindVertexArray(vao);
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0); // cantidad de indices
	glBindVertexArray(0);
}

void Mesh::decomposeModelMatrix(glm::mat4 model)
{
	/*
		Used when loading model from assimp
	*/
	glm::decompose(model, scale, rotation, translation, skew, perspective);
	rotation = glm::conjugate(rotation);
}

bool Mesh::isOnFrustum(std::shared_ptr<Frustum> frustum)
{
	//Get global scale thanks to our transform
	const glm::vec3 globalCenter{ model * glm::vec4(center, 1.f) };

	// Scaled orientation
	const glm::vec3 right = model[0] * extents.x;
	const glm::vec3 up = model[1] * extents.y;
	const glm::vec3 forward = -model[2] * extents.z;

	const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
		std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
		std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

	const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));
	const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));
	const glm::vec3 newExtents = { newIi, newIj, newIk };

	return isOnFrustum(globalCenter, newExtents, frustum);
}

bool Mesh::isOnFrustum(glm::vec3 center, glm::vec3 extents, std::shared_ptr<Frustum> frustum)
{
	return frustum->isBoxVisible(center - extents, center + extents);

#if 0
	bool left = isOnOrForwardPlane(center, extents, frustum->leftFace);
	bool right = isOnOrForwardPlane(center, extents, frustum->rightFace);
	bool top = isOnOrForwardPlane(center, extents, frustum->topFace);
	bool bottom = isOnOrForwardPlane(center, extents, frustum->bottomFace);
	bool near = isOnOrForwardPlane(center, extents, frustum->nearFace);
	bool far = isOnOrForwardPlane(center, extents, frustum->farFace);

	bool res = left && right && top && bottom && near && far;


	return isOnOrForwardPlane(center, extents, frustum->leftFace) &&
		isOnOrForwardPlane(center, extents, frustum->rightFace) &&
		isOnOrForwardPlane(center, extents, frustum->topFace) &&
		isOnOrForwardPlane(center, extents, frustum->bottomFace) &&
		isOnOrForwardPlane(center, extents, frustum->nearFace) &&
		isOnOrForwardPlane(center, extents, frustum->farFace);
#endif
}

float* Vertex::toVBO(const std::vector<Vertex>& vertices)
{
	float* vboLOD1 = new float[vertices.size() * Vertex::numElementsInVBO];
	
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		const Vertex& v = vertices[i];
		vboLOD1[i * Vertex::numElementsInVBO     ] = v.Position.x;
		vboLOD1[i * Vertex::numElementsInVBO +  1] = v.Position.y;
		vboLOD1[i * Vertex::numElementsInVBO +  2] = v.Position.z;
		vboLOD1[i * Vertex::numElementsInVBO +  3] = v.Normal.x;
		vboLOD1[i * Vertex::numElementsInVBO +  4] = v.Normal.y;
		vboLOD1[i * Vertex::numElementsInVBO +  5] = v.Normal.z;
		vboLOD1[i * Vertex::numElementsInVBO +  6] = v.TexCoords.x;
		vboLOD1[i * Vertex::numElementsInVBO +  7] = v.TexCoords.y;
		vboLOD1[i * Vertex::numElementsInVBO +  8] = v.Tangent.x;
		vboLOD1[i * Vertex::numElementsInVBO +  9] = v.Tangent.y;
		vboLOD1[i * Vertex::numElementsInVBO + 10] = v.Tangent.z;
		vboLOD1[i * Vertex::numElementsInVBO + 11] = v.Bitangent.x;
		vboLOD1[i * Vertex::numElementsInVBO + 12] = v.Bitangent.y;
		vboLOD1[i * Vertex::numElementsInVBO + 13] = v.Bitangent.z;

	}

	return vboLOD1;
}

void Mesh::setClipPlane(const glm::vec4& plane)
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

	bind(GL_FILL);

}

void Mesh::renderAABB()
{
	debugShader->use();
	debugShader->setFloat("color", 1.f, 0.f, 0.f);
	debugShader->setMat4("model", model);

	glBindVertexArray(debugVao);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
	glDrawElements(GL_LINES, debugIndicesSize, GL_UNSIGNED_INT, 0); // cantidad de indices
	glBindVertexArray(0);
}

void Mesh::renderWireframe()
{
	debugShader->use();
	debugShader->setFloat("color", 0.f, 0.f, 1.f);
	debugShader->setMat4("model", model);

	bind(GL_LINE);
}




