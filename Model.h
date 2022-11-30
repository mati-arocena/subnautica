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

protected:
    std::shared_ptr<Frustum> frustumLOD0;
    std::shared_ptr<Frustum> frustumLOD1;
    std::shared_ptr<Frustum> frustumLOD2;

	std::vector<Mesh> meshesLOD0;
    std::vector<Mesh> meshesLOD1;
    std::vector<Mesh> meshesLOD2;
	std::string directory;
    std::vector<Texture*> textures_loaded;
    
    void loadModel(const std::string& path, LOD lod);
    void processNode(aiNode* node, const aiScene* scene, const glm::mat4& transformMat, LOD lod);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene, const glm::mat4& transformMat);
    std::vector<Texture*> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);
public:
    Model(std::string path, std::string extension);

    void clipModel(const glm::vec4& plane);
    void update(double DeltaTime) override;
    void render() override;
    void renderAABB() override; 
    void renderWireframe() override;
    void renderOclussion();
    void render_withShader(std::shared_ptr<Shader> shader);
    void move(const glm::vec3& movement);
    void rotate(const glm::vec3& rotationAxis, float angle);
};

