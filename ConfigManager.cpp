#include "ConfigManager.h"

ConfigManager::ConfigManager()
{
	windowSize = glm::ivec2(800, 600);
	far = 1000.f;
	near = 0.1f;
}

ConfigManager& ConfigManager::getInstance()
{
	static ConfigManager instance;
	return instance;
}

// Setters

void ConfigManager::setWindowSize(glm::ivec2 windowSize)
{
	this->windowSize = windowSize;
}

void ConfigManager::setOcclusionMapResolution(glm::ivec2 occlusionMapResolution)
{
	this->occlusionMapResolution = occlusionMapResolution;
}

void ConfigManager::setWaterReflectionResolution(glm::ivec2 waterReflectionResolution)
{
	this->waterReflectionResolution = waterReflectionResolution;
}

void ConfigManager::setClearColor(glm::vec3 clearColor)
{
	this->clearColor = clearColor;
}

void ConfigManager::setFar(float)
{
	this->far = far;
}

void ConfigManager::setNear(float)
{
	this->near = near;
}

// Getters

glm::ivec2 ConfigManager::getWindowSize()
{
	return windowSize;
}

glm::ivec2 ConfigManager::getOcclusionMapResolution()
{
	return occlusionMapResolution;
}

glm::ivec2 ConfigManager::getWaterReflectionResolution()
{
	return waterReflectionResolution;
}

glm::vec3 ConfigManager::getClearColor()
{
	return clearColor;
}

float ConfigManager::getFar()
{
	return far;
}

float ConfigManager::getNear()
{
	return near;
}
