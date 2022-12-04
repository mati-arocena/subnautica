#include "ShadowMapBuffer.h"
#include "Definitions.h"
#include "GameInstance.h"

ShadowMapBuffer::ShadowMapBuffer(std::shared_ptr<Shader> shader)
{
	this->shader = shader;
	glGenFramebuffers(1, &shadowFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);
	
	glm::ivec2 window = ConfigManager::getInstance().getWindowSize();

	shadowDepthTexture = std::make_shared<Texture>(window.x, window.y, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, "shadow_depth", GL_DEPTH_ATTACHMENT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window.x, window.y);

}


void ShadowMapBuffer::bind()
{
	glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);
}

void ShadowMapBuffer::unbind()
{
	glm::ivec2 window = ConfigManager::getInstance().getWindowSize();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window.x, window.y);
}