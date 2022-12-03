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

class Mesh
{

public:
	// mesh data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Material* material;

	~Mesh() {};

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
		Material* material, glm::mat4 modelMat, glm::vec3 AABBmin, glm::vec3 AABBmax, bool isMovable = false);
	void render();
	void renderAABB();
	void renderWireframe();

	void updateAABB();

	void setClipPlane(const glm::vec4& plane);
	glm::vec4 getClipPlane();


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
//		this->model = r;
	}

	void decomposeModelMatrix(glm::mat4 model);

    bool isOnFrustum(std::shared_ptr<Frustum> frustum);
    void setAnimator(std::shared_ptr<Animator> animator);

	void update(float delta);

	bool isMovable();
	void recalculateAABB();


private:
	bool movable = false;

	VBO* vbo;
	unsigned int vao, ebo;
	glm::vec4 clipPlane;

	glm::mat4 model; // your transformation matrix.
	std::shared_ptr<Shader> debugShader;
	unsigned int debugIndicesSize, debugVao, debugEbo;
	VBO* debugVBO;

	glm::vec3 center;
	glm::vec3 extents;
	glm::vec3 minAABB;
	glm::vec3 maxAABB;

	float angle;

    void setupMesh();
    void bind(GLenum polygonMode);
    std::shared_ptr<Animator> animator;
    
	bool isOnFrustum(glm::vec3 center, glm::vec3 extents, std::shared_ptr<Frustum> frustum);
};

