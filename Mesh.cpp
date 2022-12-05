#include "Mesh.h"
#include "GameInstance.h"
#include <GLFW/glfw3.h>
#include "Definitions.h"
#include "Animator.h"
#include <limits>
#include "OrbitAnimation.h"

int Vertex::numElementsInVBO = 14;

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices,
	Material* material, glm::mat4 modelMat, glm::vec3 AABBmin, glm::vec3 AABBmax, MeshType type)
	: vertices(vertices), indices(indices), material(material), model(modelMat), minAABB(AABBmin), maxAABB(AABBmax),  clipPlane{}, angle{0.f}, type{type}
{
	decomposeModelMatrix(modelMat);
	computeModelMatrix();

	recalculateAABB();

	vbo = new VBO();
	debugVBO = new VBO();

	setupMesh();

	debugShader = GameInstance::getInstance().getShader(FRUSTUM_SHADER);
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

	debugVBO->load(verticesAABB, verticesAABB.size() * Vertex::numElementsInVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, debugEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * debugIndicesSize, VBO::toEBO(indicesAABB), GL_DYNAMIC_DRAW);
	
	Vertex::setVertexAttributes();
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
	if (this->globalAnimation)
	{
		this->model = this->globalAnimation->getModelMatrix(delta);
		decomposeModelMatrix(this->model);
		
	}
	else
	{
		computeModelMatrix();
	}
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

void Mesh::recalculateAABB()
{
	float a, b;
	glm::vec3 min, max;
	int    i, j;

	/* Take care of translation by beginning at T. */

	min = max = translation;

	/* Now find the extreme points by considering the product of the */
	/* min and max with each component of M.  */
	glm::mat4 rot = glm::toMat4(rotation);
	glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 M = s * rot;

	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
		{
			a = (float)(M[i][j] * minAABB[j]);
			b = (float)(M[i][j] * maxAABB[j]);
			if (a < b)

			{
				min[i] += a;
				max[i] += b;
			}
			else
			{
				min[i] += b;
				max[i] += a;
			}
		}

	/* Copy the result into the new box. */
	this->center = glm::vec3({ (max + min) * 0.5f });
	this->extents = glm::vec3({ max.x - this->center.x, max.y - this->center.y, max.z - this->center.z });
}


glm::vec3 Mesh::getCollisionDelta(const glm::vec3& center, const glm::vec3& extents)
{

	// https://gamedev.stackexchange.com/questions/129446/how-can-i-calculate-the-penetration-depth-between-two-colliding-3d-aabbs

	// Minimum Translation Vector
	// ==========================
	float resDistance = std::numeric_limits<float>::max();             // Set current minimum distance (max float value so next value is always less)
	glm::vec3 resAxis = {};                // Axis along which to travel with the minimum distance

	// Axes of potential separation
	// ============================
	// - Each shape must be projected on these axes to test for intersection:
	//          
	// (1, 0, 0)                    A0 (= B0) [X Axis]
	// (0, 1, 0)                    A1 (= B1) [Y Axis]
	// (0, 0, 1)                    A1 (= B2) [Z Axis]

	const glm::vec3 thisMax = this->center + this->extents;
	const glm::vec3 thisMin = this->center - this->extents;
	const glm::vec3 playerMax = center + extents;
	const glm::vec3 playerMin = center - extents;

	// [X Axis]
	if (!collisionTest({1.f, 0.f, 0.f}, thisMin.x, thisMax.x, playerMin.x, playerMax.x, resAxis, resDistance))
		return { 0.f, 0.f, 0.f };

	// [Y Axis]
	if (!collisionTest({ 0.f, 1.f, 0.f }, thisMin.y, thisMax.y, playerMin.y, playerMax.y, resAxis, resDistance))
		return { 0.f, 0.f, 0.f };


	// [Z Axis]
	if (!collisionTest({ 0.f, 0.f, 1.f }, thisMin.z, thisMax.z, playerMin.z, playerMax.z, resAxis, resDistance))
		return { 0.f, 0.f, 0.f };


	return glm::normalize(resAxis) * glm::sqrt(resDistance) * -1.001f;
}



bool Mesh::collisionTest(glm::vec3 axis, float minA, float maxA, float minB, float maxB, glm::vec3& resAxis, float& resDistance)
{
	// Separating Axis Theorem
	// =======================
	// - Two convex shapes only overlap if they overlap on all axes of separation
	// - In order to create accurate responses we need to find the collision vector (Minimum Translation Vector)   
	// - The collision vector is made from a vector and a scalar, 
	//   - The vector value is the axis associated with the smallest penetration
	//   - The scalar value is the smallest penetration value
	// - Find if the two boxes intersect along a single axis
	// - Compute the intersection interval for that axis
	// - Keep the smallest intersection/penetration value
	float axisLengthSquared = glm::dot(axis, axis);

	// If the axis is degenerate then ignore
	if (axisLengthSquared < 1.0e-8f)
		return true;

	// Calculate the two possible overlap ranges
	// Either we overlap on the left or the right sides
	float d0 = (maxB - minA);   // 'Left' side
	float d1 = (maxA - minB);   // 'Right' side

	// Intervals do not overlap, so no intersection
	if (d0 <= 0.0f || d1 <= 0.0f)
		return false;

	// Find out if we overlap on the 'right' or 'left' of the object.
	float overlap = (d0 < d1) ? d0 : -d1;

	// The mtd vector for that axis
	glm::vec3 sep = axis * (overlap / axisLengthSquared);

	// The mtd vector length squared
	float sepLengthSquared = glm::dot(sep, sep);

	// If that vector is smaller than our computed Minimum Translation Distance use that vector as our current MTV distance
	if (sepLengthSquared < resDistance)
	{
		resDistance = sepLengthSquared;
		resAxis = sep;
	}

	return true;

}

glm::vec3 Mesh::getAABBCenter() const
{
	return center;
}

glm::vec3 Mesh::getAABBExtents() const
{
	return extents;
}

bool Mesh::hasCollision() const
{
	return type == MeshType::COLLISION;
}

bool Mesh::isParticle() const
{
	return type == MeshType::PARTICLE;
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
	return frustum->isBoxInFrustum(center - extents, center + extents);
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

glm::vec4 Mesh::getClipPlane() const
{
	return this->clipPlane;
}

void Mesh::setGlobalAnimation(std::shared_ptr<GlobalAnimation> globalAnimation)
{
	this->globalAnimation = globalAnimation;
}

void Mesh::render_withShader(std::shared_ptr<Shader> shader)
{
	shader->use();
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

void Mesh::setModel(glm::mat4 model)
{
	this->model = model;
}


void Mesh::renderAABB()
{
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

	debugVBO->updateDynamic(verticesAABB);

	debugShader->use();
	debugShader->setFloat("color", 1.f, 0.f, 0.f);
	glm::mat4 identity = glm::identity<glm::mat4>();
	debugShader->setMat4("model", identity);

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

void Mesh::updateAABB()
{
	glm::vec3 worldMin = glm::vec3(model * glm::vec4(minAABB, 1.f));
	glm::vec3 worldMax = glm::vec3(model * glm::vec4(maxAABB, 1.f));
	center = { (worldMax + worldMin) * 0.5f };
	extents = { worldMax.x - center.x, worldMax.y - center.y, worldMax.z - center.z };
}