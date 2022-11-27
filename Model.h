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
    std::vector<std::shared_ptr<Texture>> textures_loaded;
    std::vector<std::shared_ptr<btRigidBody>> collisionObjects;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene, glm::mat4 transformMat);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 transformMat);
    std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    void decomposeGLMMatrix(const glm::mat4& m, glm::vec3& pos, glm::quat& rot);
public:
    Model(std::string path);
    ~Model();

    void clipModel(glm::vec4 plane);
    void update(double DeltaTime) override;
    void renderLOD(LOD levelOfDetail) override;
    void renderOclussion();
    void render_withShader(std::shared_ptr<Shader> shader);
    std::vector<std::shared_ptr<btRigidBody>> getCollisionObject();
};

