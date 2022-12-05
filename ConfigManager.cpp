#include "ConfigManager.h"

ConfigManager::ConfigManager() : windowSize {glm::ivec2(800, 600)}, far {200.f}, near {0.1f}
{
}

ConfigManager& ConfigManager::getInstance()
{
	static ConfigManager instance;
	return instance;
}

// Setters

void ConfigManager::setWindowSize(const glm::ivec2& windowSize)
{
	this->windowSize = windowSize;
}

void ConfigManager::setOcclusionMapResolution(const glm::ivec2& occlusionMapResolution)
{
	this->occlusionMapResolution = occlusionMapResolution;
}

void ConfigManager::setWaterReflectionResolution(const glm::ivec2& waterReflectionResolution)
{
	this->waterReflectionResolution = waterReflectionResolution;
}

void ConfigManager::setClearColor(const glm::vec3& clearColor)
{
	this->clearColor = clearColor;
}

void ConfigManager::setFar(float far)
{
	this->far = far;
}

void ConfigManager::setNear(float near)
{
	this->near = near;
}

void ConfigManager::setMode(Mode mode)
{
	this->mode = mode;
}

Mode ConfigManager::getMode() const
{
	return this->mode;
}

// Getters

glm::ivec2 ConfigManager::getWindowSize() const
{
	return windowSize;
}

glm::ivec2 ConfigManager::getOcclusionMapResolution() const
{
	return occlusionMapResolution;
}

glm::ivec2 ConfigManager::getWaterReflectionResolution() const
{
	return waterReflectionResolution;
}

glm::vec3 ConfigManager::getClearColor() const
{
	return clearColor;
}

float ConfigManager::getFar() const
{
	return far;
}

float ConfigManager::getNear() const
{
	return near;
}
