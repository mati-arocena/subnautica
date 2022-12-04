#pragma once
#include "Texture.h"
#include "Shader.h"


class ShadowMapBuffer
{

public:
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Texture> shadowDepthTexture;
	unsigned int shadowFrameBuffer;

	explicit ShadowMapBuffer(std::shared_ptr<Shader> shader);

	void bind();
	static void unbind();
};

