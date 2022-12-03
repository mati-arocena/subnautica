#pragma once
#include "Texture.h"
#include "Shader.h"


class ShadowMapBuffer
{
	const int WIDTH = 800;
	const int HEIGHT = 600;

public:
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Texture> shadowDepthTexture;
	unsigned int shadowFrameBuffer;

	ShadowMapBuffer(std::shared_ptr<Shader> shader);

	void bind();
	void unbind();
};

