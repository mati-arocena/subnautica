#pragma once
#include <memory>
#include <glm/glm.hpp>


class Particle
{

private:
    glm::vec3 position, velocity;
    glm::vec4 color;
    float life;
    float scale;
    float elapsedTime = 0;
    float gravityEffect;

public:

    Particle(glm::vec3 position, glm::vec3 velocity, glm::vec4 color, float life, float scale, float gravityEffect);
    glm::vec3 getPosition();
    glm::vec3 getRotation();
    glm::vec3 getScale();
    glm::vec4 getColor();

    bool update(double deltaTime);
    glm::mat4 getModel();

};

