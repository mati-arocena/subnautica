#include "PostProcessor.h"
#include "GameInstance.h"
#include "Definitions.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

void PostProcessor::initVertexBuffers()
{
    // configure VAO/VBO for postprocessing
    float vertices[] = {
        // pos        // tex
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}



PostProcessor::PostProcessor()
{
    // Initialize opengl vertex for postprocessing (Maybe migrate to postprocessing class)
    initVertexBuffers();

    occlusion_TX = NULL;
    occlusionShader = GameInstance::getInstance().getShader(OCCLUSION_SHADER);

    scene_TX = NULL;
    postProcessingShader = GameInstance::getInstance().getShader(POST_SHADER);

    // Occlusion Buffer
    glGenFramebuffers(1, &occlusion_FB);
    glBindFramebuffer(GL_FRAMEBUFFER, occlusion_FB);
    occlusion_TX = new Texture(200, 150, GL_RGB, GL_RGB, "occlusion_texture", GL_COLOR_ATTACHMENT0);

    // Scene Buffer
    glGenFramebuffers(1, &scene_FB);
    glBindFramebuffer(GL_FRAMEBUFFER, scene_FB);

    scene_TX = new Texture(800, 600, GL_RGB, GL_RGB, "scene_texture", GL_COLOR_ATTACHMENT0);
    sceneDepth_TX = new Texture(800, 600, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, "scene_texture_depth", GL_DEPTH_ATTACHMENT);

    postProcessingShader->setTexture(occlusion_TX, 0);
    postProcessingShader->setTexture(scene_TX, 1);
    postProcessingShader->setTexture(sceneDepth_TX, 2);
}

void PostProcessor::draw()
{
    // RUN

    // Render de occlussion
    glBindTexture(GL_TEXTURE_2D, 0);
    glViewport(0, 0, 200, 150);
    glBindFramebuffer(GL_FRAMEBUFFER, occlusion_FB);
    GameInstance::getInstance().render_withShader(occlusionShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 800, 600);

    // Blend
    postProcessingShader->use();
    occlusion_TX->use(0);
    float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    scene_TX->use(1);
    sceneDepth_TX->use(2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    auto light = GameInstance::getInstance().getLight();
    auto camera = GameInstance::getInstance().getCamera();

    glm::mat4 projection = camera->GetProjectionMatrix();
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 model = glm::identity<glm::mat4>();

    glm::vec3 projected = view * glm::vec4(light->getPosition(), 1);// glm::project(light->getPosition(), model, projection, glm::vec4(0.f, 0.f, 200, 150));

    postProcessingShader->setFloat("lightPos_SS", projected.x, projected.y);
    postProcessingShader->setFloat("time", glfwGetTime());

    glBindVertexArray(VAO);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

}

GLint PostProcessor::getSceneFB()
{
    return scene_FB;
}