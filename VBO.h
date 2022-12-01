#pragma once
#include <gl/glew.h>
#include <vector>

#include <glm/glm.hpp>

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    Vertex()
    {
        for (size_t i = 0; i < MAX_BONE_INFLUENCE; ++i)
        {
            this->m_BoneIDs[i] = -1;
            this->m_Weights[i] = 0.0f;
        }

    }
    Vertex(glm::vec3 Position, glm::vec3 Normal, glm::vec2 TexCoords) {
        this->Position = Position;
        this->Normal = Normal;
        this->TexCoords = TexCoords;

        this->Bitangent = glm::vec3(0.0f, 0.0f, 0.0f);
        this->Tangent = glm::vec3(0.0f, 0.0f, 0.0f);
		
        for (size_t i = 0; i < MAX_BONE_INFLUENCE; ++i)
        {
            this->m_BoneIDs[i] = 0;
            this->m_Weights[i] = 0.0f;
        }
    }
	
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;

    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];

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


class VBO
{
public:
	VBO();
	void load(float* vertices, size_t lenght);
	void load(std::vector<Vertex> vertices_v, size_t lenght);
	void bind();
	void destroy();
	~VBO();

	static unsigned int* toEBO(const std::vector<unsigned int>& indices); 
private:
	unsigned int id;
	size_t lenght;
	float* vertices;
    std::vector<Vertex> vertices_v;
	void setVertexAttribute(int id, int stride);
};

