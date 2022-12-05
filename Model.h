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

protected:
    std::shared_ptr<Frustum> frustumLOD0;
    std::shared_ptr<Frustum> frustumLOD1;
    std::shared_ptr<Frustum> frustumLOD2;

	std::vector<Mesh> meshesLOD0;
    std::vector<Mesh> meshesLOD1;
    std::vector<Mesh> meshesLOD2;
	std::string directory;
    std::vector<std::shared_ptr<Texture>> textures_loaded;
    
    bool hasAnimations = false;
    void loadModel(const std::string& path, LOD lod);
    void loadAnimations(std::string path);

    void readMissingBones(const aiAnimation* assimpAnim, std::shared_ptr<Animation> animation);
    void processNode(aiNode* node, const aiScene* scene, const glm::mat4& transformMat, LOD lod);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& transformMat);
    std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);

    std::shared_ptr<Animator> animator;
    std::vector<std::shared_ptr<Animation>> animations;

    int m_BoneCounter = 0;
    std::shared_ptr<std::map<std::string, BoneInfo>> m_BoneInfoMap;
    void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh);
    
    bool isMovable = false;
    glm::vec3 scale;
    glm::vec3 rotation;
    glm::vec3 position;
public:
    Model(const std::string& path, const std::string& extension, glm::vec3 position);
    Model(const std::string& path, const std::string& extension, const std::string& animationPath, const std::string& animationExtension, glm::vec3 position);

    void clipModel(const glm::vec4& plane);
    void update(double DeltaTime) override;
    void render() override;
    void renderAABB() override; 
    void renderWireframe() override;
    void renderOclussion() override;
    void render_withShader(std::shared_ptr<Shader> shader) override;
    void move(const glm::vec3& movement);
    void rotate(const glm::vec3& rotationAxis, float angle);
    int& getBoneCount();

    void setAnimator(std::shared_ptr<Animator> animator);

	void setGlobalAnimation(std::shared_ptr<GlobalAnimation> globalAnimation);
};

