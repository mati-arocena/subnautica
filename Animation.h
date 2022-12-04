#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Utils.h"
#include <assimp/Importer.hpp>

#include "AssimpNodeData.h"
#include "BoneInfo.h"

#include "Bone.h"

#include <map>
#include <memory>
#include <assimp/anim.h>
#include <assimp/scene.h>


class Animation
{
public:
    Animation() = default;

    Animation(aiNode* mRootNode, aiAnimation* animationNode);

    ~Animation();

    Bone* findBone(const std::string& name);

    float getTicksPerSecond() const;

    float getDuration() const;

    const AssimpNodeData& getRootNode() const;

    const std::map<std::string, BoneInfo>& getBoneIDMap() const;

    void addBone(std::shared_ptr<Bone> bone);

    void setBoneInfoMap(std::shared_ptr<std::map<std::string, BoneInfo>> m_BoneInfoMap);


private:

    void readHeirarchyData(AssimpNodeData& dest, const aiNode* src);

    float m_Duration = 0.f;
    int m_TicksPerSecond = 0;
    std::vector<Bone> m_Bones;
    AssimpNodeData m_RootNode;
    std::shared_ptr<std::map<std::string, BoneInfo>> m_BoneInfoMap;

};
