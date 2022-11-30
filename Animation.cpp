#include "Animation.h"

Animation::Animation(aiNode* mRootNode, aiAnimation* animationNode)
{
    m_Duration = animationNode->mDuration;
    m_TicksPerSecond = animationNode->mTicksPerSecond;
    readHeirarchyData(m_RootNode, mRootNode);
}

Animation::~Animation()
{
}

Bone* Animation::findBone(const std::string& name)
{
    auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
        [&](const Bone& Bone)
        {
            return Bone.getBoneName() == name;
        }
    );
    if (iter == m_Bones.end()) return nullptr;
    else return &(*iter);
}

float Animation::getTicksPerSecond()
{
	return m_TicksPerSecond;
}

float Animation::getDuration()
{
	return m_Duration;
}

const AssimpNodeData& Animation::getRootNode()
{
	return m_RootNode;
}

const std::map<std::string, BoneInfo>& Animation::getBoneIDMap()
{
	return *m_BoneInfoMap;
}


void Animation::addBone(std::shared_ptr<Bone> bone)
{
	m_Bones.push_back(*bone);
}

void Animation::setBoneInfoMap(std::shared_ptr<std::map<std::string, BoneInfo>> m_BoneInfoMap)
{
    this->m_BoneInfoMap = m_BoneInfoMap;
}

void Animation::readHeirarchyData(AssimpNodeData& dest, const aiNode* src)
{
    assert(src);

    dest.name = src->mName.data;
    dest.transformation = convertMatrix(src->mTransformation);
    dest.childrenCount = src->mNumChildren;

    for (int i = 0; i < src->mNumChildren; i++)
    {
        AssimpNodeData newData;
        readHeirarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}
