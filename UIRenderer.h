#pragma once
#include "Shader.h"

#include <memory>

class UIRenderer
{
public:
    UIRenderer(std::shared_ptr<Shader> shader);
    ~UIRenderer();

    void draw(Texture* texture, const glm::vec2& position,
        const glm::vec2& size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f,
        const glm::vec3& color = glm::vec3(1.0f));
private:
    std::shared_ptr<Shader> shader;
    unsigned int quadVAO;

    void initRenderData();

};

