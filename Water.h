#pragma once
#include <glm/glm.hpp>
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Model.h"


class Water : public GameObject
{
private:
	glm::vec3 position;
	Mesh* mesh;
	std::shared_ptr<Texture> refrTexture;
	std::shared_ptr<Texture> reflTexture;
	std::shared_ptr<Texture> reflectionDepthTexture;
	std::shared_ptr<Texture> refractionDepthTexture;
	std::shared_ptr<Texture> dudv_texture;
	unsigned int reflFrameBuffer;
	unsigned int refrFrameBuffer;

	glm::ivec2 waterReflectionResolution;

public:
	Water();
	void update(double deltaTime) override;
	void render() override;
	void renderOclussion() override;

	static void unbindFrameFuffer();
	Material* initializeMaterial();
	void render_withShader(std::shared_ptr<Shader> shader) override;

};

