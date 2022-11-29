#pragma once

#include "Animation.h"

class Animator
{
public:
    Animator(Animation* animation);

    void updateAnimation(float dt);

    void playAnimation(Animation* pAnimation);

    void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

    std::vector<glm::mat4>* getFinalBoneMatrices();
	
    void addAnimation(std::shared_ptr<Animation> animation);
	std::shared_ptr<Animation> getAnimation(std::string name);

private:
    std::vector<glm::mat4> m_FinalBoneMatrices;
    Animation* m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;

    std::vector<std::shared_ptr<Animation>> m_Animations;
};

