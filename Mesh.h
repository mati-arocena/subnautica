#pragma once
#include <vector>
#include "Material.h"
#include "VBO.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

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
};

class Mesh
{

public:
    // mesh data
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material* material;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material* material, glm::mat4 modelMat);
    void render();
    void setClipPlane(glm::vec4 plane);
    glm::vec4 getClipPlane();
    glm::mat4 model;
    void render_withShader(std::shared_ptr<Shader> shader);

private:
    VBO* vbo;
    unsigned int vao, ebo;
    glm::vec4 clipPlane;

    void setupMesh();
    unsigned int* ToEBO();
};

