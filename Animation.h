#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Model.h"
#include "Bone.h"
#include "Utils.h"
#include <assimp/Importer.hpp>

struct AssimpNodeData
{
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};

class Animation
{
public:
    Animation() = default;

    Animation(const std::string& animationPath, std::shared_ptr<Model> model);

    ~Animation();

    Bone* findBone(const std::string& name);

    float getTicksPerSecond();

    float getDuration();

    const AssimpNodeData& getRootNode();

    const std::map<std::string, BoneInfo>& getBoneIDMap();


	

private:
    void readMissingBones(const aiAnimation* animation, Model& model);

    void readHeirarchyData(AssimpNodeData& dest, const aiNode* src);
    float m_Duration;
    int m_TicksPerSecond;
    std::vector<Bone> m_Bones;
    AssimpNodeData m_RootNode;
    std::map<std::string, BoneInfo> m_BoneInfoMap;

};
