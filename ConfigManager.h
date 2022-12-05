#pragma once


#include <memory>
#include <glm/glm.hpp>


enum Mode {
	PLAYER_MODE,
	FLY_MODE
};

class ConfigManager
{
private:
	ConfigManager();
	glm::ivec2 windowSize = glm::ivec2(1080,720);
	glm::ivec2 occlusionMapResolution = glm::ivec2(200, 150);
	glm::ivec2 waterReflectionResolution = glm::ivec2(200, 150);
	
	glm::vec3 clearColor = glm::vec3(0, 0, 0);
	
	float far = 200.f;
	float near = 0.01f;
	Mode mode = Mode::FLY_MODE;

public:
	static ConfigManager& getInstance();

	void operator=(ConfigManager const&) = delete;
	ConfigManager(ConfigManager const&) = delete;

	void setWindowSize(const glm::ivec2& windowSize);
	void setOcclusionMapResolution(const glm::ivec2& occlusionMapResolution);
	void setWaterReflectionResolution(const glm::ivec2& waterReflectionResolution);

	void setClearColor(const glm::vec3& clearColor);
	
	void setFar(float far);
	void setNear(float near);

	void setMode(Mode mode);
	Mode getMode() const;

	glm::ivec2 getWindowSize() const;
	glm::ivec2 getOcclusionMapResolution() const;
	glm::ivec2 getWaterReflectionResolution() const;

	glm::vec3 getClearColor() const;

	float getFar() const;
	float getNear() const;
};

