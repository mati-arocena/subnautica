#include "ShadowMapBuffer.h"
#include "Definitions.h"
#include "GameInstance.h"

ShadowMapBuffer::ShadowMapBuffer(std::shared_ptr<Shader> shader)
{
	this->shader = shader;
	glGenFramebuffers(1, &shadowFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);

	shadowDepthTexture = std::make_shared<Texture>(WIDTH, HEIGHT, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, "shadow_depth", GL_DEPTH_ATTACHMENT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WIDTH, HEIGHT);

}


void ShadowMapBuffer::bind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);
}

void ShadowMapBuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WIDTH, HEIGHT);
}