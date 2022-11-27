#pragma once
#include "Shader.h"

#include <memory>

class UIRenderer
{
public:
    UIRenderer(std::shared_ptr<Shader> shader);
    ~UIRenderer();

    void draw(std::shared_ptr<Texture> texture, glm::vec2 position,
        glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f,
        glm::vec3 color = glm::vec3(1.0f));
private:
    std::shared_ptr<Shader> shader;
    unsigned int quadVAO;

    void initRenderData();

};

