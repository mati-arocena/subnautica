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

    static float* toVBO(std::vector<Vertex> verticesLOD0);
    static int numElementsInVBO;
};

class Mesh
{

public:
    // mesh data
    std::vector<Vertex> verticesLOD0;
    std::vector<unsigned int> indicesLOD0;
    std::vector<Vertex> verticesLOD1;
    std::vector<unsigned int> indicesLOD1;
    std::vector<Vertex> verticesLOD2;
    std::vector<unsigned int> indicesLOD2;
    Material* material;


    Mesh(std::vector<Vertex> verticesLOD0, std::vector<unsigned int> indicesLOD0, 
        std::vector<Vertex> verticesLOD1, std::vector<unsigned int> indicesLOD1,
        std::vector<Vertex> verticesLOD2, std::vector<unsigned int> indicesLOD2,
        Material* material, glm::mat4 modelMat, glm::vec3 AABBmin, glm::vec3 AABBmax);
    void render();
    void setClipPlane(glm::vec4 plane);
    glm::vec4 getClipPlane();
    glm::mat4 model;
    void render_withShader(std::shared_ptr<Shader> shader);
    void toggleDebugAABB();

    inline void move(glm::vec3 transform)
    {
        model = glm::translate(model, transform);
    }

    inline void rotate(glm::vec3 rotationAxis, float angle)
    {
        model = glm::rotate(model, angle, rotationAxis);
    }
private:
    VBO* vboLOD0, *vboLOD1, *vboLOD2;
    unsigned int vaoLOD0, eboLOD0, vaoLOD1, eboLOD1, vaoLOD2, eboLOD2;
    glm::vec4 clipPlane;

    std::shared_ptr<Frustum> frustumLOD0;
    std::shared_ptr<Frustum> frustumLOD1;
    std::shared_ptr<Frustum> frustumLOD2;

    std::shared_ptr<Shader> debugShader; 
    unsigned int debugIndicesSize, debugVao, debugEbo;
    VBO* debugVBO;
    float debugAABB = false;

    glm::vec3 minAABB;
    glm::vec3 maxAABB;


    glm::vec3 center;
    glm::vec3 extents;

    glm::vec3 transform;
    glm::quat rotation;

    void setupMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, VBO& vbo, unsigned int& vao, unsigned int& ebo);
    void bindToLOD(LOD lod);
    
    LOD getLOD();
    bool isOnFrustum(glm::vec3 minAABB, glm::vec3 maxAABB, std::shared_ptr<Frustum> frustum);
};

