#pragma once
#include <vector>
#include "Material.h"
#include "VBO.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

enum class LOD
{
    LOD0,
    LOD1,
    LOD2,
    NotInFrustum
};

struct Vertex {
    Vertex(glm::vec3 Position, glm::vec3 Normal, glm::vec2 TexCoords) {
        this->Position = Position;
        this->Normal = Normal;
        this->TexCoords = TexCoords;

        this->Bitangent = glm::vec3(0.0f, 0.0f, 0.0f);
        this->Tangent = glm::vec3(0.0f, 0.0f, 0.0f);
    }
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
	glm::vec3 Bitangent;

    static float* toVBO(std::vector<Vertex> vertices);
    static int numElementsInVBO;
};

class Mesh
{

public:
    // mesh data
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material* material;


    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, 
        Material* material, glm::mat4 modelMat, glm::vec3 AABBmin, glm::vec3 AABBmax);
    void render();
    void renderAABB();
    void renderWireframe();

    void setClipPlane(glm::vec4 plane);
    glm::vec4 getClipPlane();
    glm::mat4 model;
    void render_withShader(std::shared_ptr<Shader> shader);

    inline void move(glm::vec3 transform)
    {
        model = glm::translate(model, transform);
    }

    inline void rotate(glm::vec3 rotationAxis, float angle)
    {
        model = glm::rotate(model, angle, rotationAxis);
    }

    bool isOnFrustum(std::shared_ptr<Frustum> frustum);
private:
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

    glm::vec3 transform;
    glm::quat rotation;

    void setupMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, VBO& vbo, unsigned int& vao, unsigned int& ebo);
    void bind(GLenum polygonMode);
    
    bool isOnFrustum(glm::vec3 center, glm::vec3 extents, std::shared_ptr<Frustum> frustum);
    inline bool isOnOrForwardPlane(glm::vec3 AABBcenter, glm::vec3 AABBextents, const Plane& plane) {
        // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
        const float r = AABBextents.x * std::abs(plane.normal.x) +
            AABBextents.y * std::abs(plane.normal.y) + AABBextents.z * std::abs(plane.normal.z);

        
        return -r <= plane.getSignedDistanceToPlan(AABBcenter);
           
    }
};

