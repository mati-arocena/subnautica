#include "Animator.h"

Animator::Animator(std::shared_ptr<Animation> animation)
{
	m_CurrentTime = 0.0;
	m_CurrentAnimation = animation;

	m_FinalBoneMatrices.reserve(100);

	for (int i = 0; i < 100; i++)
		m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
}

void Animator::updateAnimation(float dt)
{
    m_DeltaTime = dt;
    if (m_CurrentAnimation)
    {
        m_CurrentTime += m_CurrentAnimation->getTicksPerSecond() * dt;
        m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->getDuration());
        calculateBoneTransform(&m_CurrentAnimation->getRootNode(), glm::mat4(1.0f));
    }
}

void Animator::playAnimation(std::shared_ptr<Animation> pAnimation)
{
    m_CurrentAnimation = pAnimation;
    m_CurrentTime = 0.0f;
}

void Animator::calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
    const std::string* nodeName = &(node->name);
    glm::mat4 nodeTransform = node->transformation;

    Bone* Bone = m_CurrentAnimation->findBone(*nodeName);

    if (Bone)
    {
        Bone->update(m_CurrentTime);
        nodeTransform = Bone->getLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    std::map<std::string, BoneInfo> boneInfoMap = m_CurrentAnimation->getBoneIDMap();
    if (boneInfoMap.find(*nodeName) != boneInfoMap.end())
    {
        int index = boneInfoMap[*nodeName].id;
        glm::mat4 offset = boneInfoMap[*nodeName].offset;
        m_FinalBoneMatrices[index] = globalTransformation * offset;
    }

    for (int i = 0; i < node->childrenCount; i++)
        calculateBoneTransform(&node->children[i], globalTransformation);
}

std::vector<glm::mat4>* Animator::getFinalBoneMatrices()
{
    return &m_FinalBoneMatrices;
}
