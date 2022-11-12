#pragma once
#include <vector>
#include "Material.h"
#include "VBO.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh
{

public:
    // mesh data
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material material;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material);
    void Draw();

private:
    VBO vbo;
    unsigned int vao, ebo;

    void setupMesh();

};

