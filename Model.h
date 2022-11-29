#pragma once

#include "GameObject.h"

#include <vector>
#include <map>
#include <string>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"

struct BoneInfo
{
    /*id is index in finalBoneMatrices*/
    int id;

    /*offset matrix transforms vertex from model space to bone space*/
    glm::mat4 offset;

};

class Model : public GameObject
{

private:
	std::vector<Mesh> meshes;
	std::string directory;
    std::vector<Texture*> textures_loaded;
    
    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene, glm::mat4 transformMat);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 transformMat);
    std::vector<Texture*> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

    
    int m_BoneCounter = 0;
    std::map<std::string, BoneInfo> m_BoneInfoMap;

    void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
	

public:
    Model(std::string path);
    ~Model();

    void clipModel(glm::vec4 plane);
    void update(double DeltaTime) override;
    void render() override;
    void renderOclussion();
    void render_withShader(std::shared_ptr<Shader> shader);
    void move(glm::vec3 movement);
    void rotate(glm::vec3 rotationAxis, float angle);
    std::map < std::string, BoneInfo>& getBoneInfoMap();
    int& getBoneCount();

    void setIsAnimation(bool isAnimation);
};

