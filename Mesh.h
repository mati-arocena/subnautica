#pragma once
#include <vector>
#include "Material.h"
#include "VBO.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "Animator.h"


enum class LOD
{
	LOD0,
	LOD1,
	LOD2,
	NotInFrustum
};

enum class MeshType
{
	NORMAL,
	COLLISION,
	PARTICLE
};

class Mesh
{

public:
	// mesh data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Material* material;

	~Mesh() {};

	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices,
		Material* material, glm::mat4 modelMat, glm::vec3 AABBmin, glm::vec3 AABBmax, MeshType type = MeshType::NORMAL);
	void render();
	void renderAABB();
	void renderWireframe();

	void updateAABB();

	void setClipPlane(const glm::vec4& plane);
	glm::vec4 getClipPlane() const;


	glm::mat4* model_P = new glm::mat4(1.f);

	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;



	void render_withShader(std::shared_ptr<Shader> shader);

	inline void move(const glm::vec3& translate)
	{
		translation += translate;
	}

    inline void rotate(const glm::vec3& rotationAxis, float angle)
    {
		rotation *= glm::angleAxis(angle, rotationAxis);
		recalculateAABB();
    }

	inline void computeModelMatrix()
	{
		glm::mat4 t = glm::translate(glm::mat4(1.0f), translation);
		glm::mat4 r = glm::toMat4(rotation);
		glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);

		this->model = t * r * s;
	}

	void decomposeModelMatrix(glm::mat4 model);

    bool isOnFrustum(std::shared_ptr<Frustum> frustum);
    void setAnimator(std::shared_ptr<Animator> animator);

	void update(float delta);

	void recalculateAABB();

	glm::vec3 getCollisionDelta(const glm::vec3& center, const glm::vec3& extents);
	glm::vec3 getAABBCenter() const;
	glm::vec3 getAABBExtents() const;

	bool hasCollision() const;
	bool isParticle() const;
private:

	VBO* vbo;
	unsigned int vao, ebo;

	glm::mat4 model; // your transformation matrix.
	std::shared_ptr<Shader> debugShader;
	unsigned int debugIndicesSize, debugVao, debugEbo;
	VBO* debugVBO;

	glm::vec3 center;
	glm::vec3 extents;
	glm::vec3 minAABB;
	glm::vec3 maxAABB;
	glm::vec4 clipPlane;

	float angle;

	MeshType type = MeshType::NORMAL;

    void setupMesh();
    void bind(GLenum polygonMode);
    std::shared_ptr<Animator> animator;
    
	bool isOnFrustum(glm::vec3 center, glm::vec3 extents, std::shared_ptr<Frustum> frustum);
	
	static bool collisionTest(glm::vec3 axis, float minA, float maxA, float minB, float maxB, glm::vec3& resAxis, float& resDistance);
};

