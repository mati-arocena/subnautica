#pragma once

#include "Animation.h"
#include "Utils.h"
#include "AssimpNodeData.h"
#include <memory>


class Animator
{
public:
    explicit Animator(std::shared_ptr<Animation> animation);

    void updateAnimation(float dt);

    void playAnimation(std::shared_ptr<Animation> pAnimation);

    void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

    std::vector<glm::mat4>* getFinalBoneMatrices();
	
    void addAnimation(std::shared_ptr<Animation> animation);
	std::shared_ptr<Animation> getAnimation(std::string name);

private:
    std::vector<glm::mat4> m_FinalBoneMatrices;
    std::shared_ptr<Animation> m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;
};

