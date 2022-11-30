#pragma once
#include <vector>
#include "Material.h"
#include "VBO.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

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

    static float* toVBO(const std::vector<Vertex>& vertices);
    static int numElementsInVBO;
    static void setVertexAttribute(int id, int stride);
    static void setVertexAttributes();
};

inline void Vertex::setVertexAttribute(int id, int stride)
{
    glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, Vertex::numElementsInVBO * sizeof(float), (void*)(stride * sizeof(float)));
    glEnableVertexAttribArray(id);

}

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

    glm::vec3 translation;
    glm::vec3 rotation;

    void setupMesh();
    void bind(GLenum polygonMode);
    
    bool isOnFrustum(glm::vec3 center, glm::vec3 extents, std::shared_ptr<Frustum> frustum);
    inline bool isOnOrForwardPlane(const glm::vec3& AABBcenter, const glm::vec3& AABBextents, const Plane& plane) {
        // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
        const float r = AABBextents.x * std::abs(plane.normal.x) +
            AABBextents.y * std::abs(plane.normal.y) + AABBextents.z * std::abs(plane.normal.z);

        
        return -r <= plane.getSignedDistanceToPlan(AABBcenter);
           
    }
};

