#pragma once

#include "GameObject.h"

#include <vector>
#include <string>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"

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
public:
    Model(std::string path);
    ~Model();

    void clipModel(glm::vec4 plane);
    void update(double DeltaTime) override;
    void render();
};

