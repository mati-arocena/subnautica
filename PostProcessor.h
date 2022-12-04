#pragma once

#include <glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Model.h"


class PostProcessor
{


public:

	PostProcessor();
	void draw();
	GLint getSceneFB();

private:
	GLuint VBO, VAO;
	GLuint occlusion_FB, scene_FB;

	std::shared_ptr<Shader> occlusionShader, postProcessingShader;

	std::shared_ptr<Texture> occlusion_TX, scene_TX, sceneDepth_TX;

	glm::ivec2 windowSize;
	glm::ivec2 occlusionMapResolution;

	void initVertexBuffers();
	void initFrameBuffers();


};