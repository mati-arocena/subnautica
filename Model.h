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

#include "BoneInfo.h"

#include "Animator.h"


class Model : public GameObject
{

private:
	std::vector<Mesh> meshes;
	std::string directory;
    std::vector<Texture*> textures_loaded;
    
    void loadModel(std::string path);
    void loadAnimations(std::string path);
	
    void readMissingBones(const aiAnimation* assimpAnim, std::shared_ptr<Animation> animation);
	
    void processNode(aiNode* node, const aiScene* scene, glm::mat4 transformMat);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 transformMat);
    std::vector<Texture*> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

    std::shared_ptr<Animator> animator;
    std::vector<std::shared_ptr<Animation>> animations;

    int m_BoneCounter = 0;
    std::shared_ptr<std::map<std::string, BoneInfo>> m_BoneInfoMap;

    void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh);
	

public:
    Model(std::string path);
    Model(std::string path, std::string animationPath);
    ~Model();

    void clipModel(glm::vec4 plane);
    void update(double DeltaTime) override;
    void render() override;
    void renderOclussion();
    void render_withShader(std::shared_ptr<Shader> shader);
    void move(glm::vec3 movement);
    void rotate(glm::vec3 rotationAxis, float angle);
    int& getBoneCount();

    void setAnimator(std::shared_ptr<Animator> animator);
};

