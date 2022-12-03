#pragma once
#include <vector>
#include "Material.h"
#include "VBO.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

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


	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
		Material* material, glm::mat4 modelMat, glm::vec3 AABBmin, glm::vec3 AABBmax, bool isMovable = false);
	void render();
	void renderAABB();
	void renderWireframe();

	void updateAABB();

	void setClipPlane(const glm::vec4& plane);
	glm::vec4 getClipPlane();
	glm::mat4 model;
	void render_withShader(std::shared_ptr<Shader> shader);

	inline void move(const glm::vec3& translate)
	{
		model = glm::translate(model, translate);
	}

    inline void rotate(const glm::vec3& rotationAxis, float angle)
    {
        //model = glm::rotate(model, glm::radians(angle), rotationAxis);
        model *= glm::eulerAngleYXZ(
            glm::radians(0.f),
            glm::radians(angle),
            glm::radians(0.f));
    }

	bool isOnFrustum(std::shared_ptr<Frustum> frustum);
    void setAnimator(std::shared_ptr<Animator> animator);

	bool isMovable();

private:
	bool movable = false;
	glm::vec3 scale;
	glm::vec3 rotation;
	glm::vec3 position;

	VBO* vbo;
	unsigned int vao, ebo;
	glm::vec4 clipPlane;

	std::shared_ptr<Shader> debugShader;
	unsigned int debugIndicesSize, debugVao, debugEbo;
	VBO* debugVBO;

	glm::vec3 center;
	glm::vec3 extents;
	glm::vec3 minAABB;
	glm::vec3 maxAABB;

	glm::vec3 translation;
	float angle;

    void setupMesh();
    void bind(GLenum polygonMode);
    std::shared_ptr<Animator> animator;
    
	bool isOnFrustum(glm::vec3 center, glm::vec3 extents, std::shared_ptr<Frustum> frustum);

};

