#pragma once


#include <memory>
#include <glm/glm.hpp>



class ConfigManager
{
private:
	ConfigManager();
	glm::ivec2 windowSize = glm::ivec2(1080,720);
	glm::ivec2 occlusionMapResolution = glm::ivec2(200, 150);
	glm::ivec2 waterReflectionResolution = glm::ivec2(200, 150);
	
	glm::vec3 clearColor = glm::vec3(0, 0, 0);
	
	float far = 100.f;
	float near = 0.01f;
	
	

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

	glm::ivec2 getWindowSize();
	glm::ivec2 getOcclusionMapResolution();
	glm::ivec2 getWaterReflectionResolution();

	glm::vec3 getClearColor();

	float getFar();
	float getNear();

};

