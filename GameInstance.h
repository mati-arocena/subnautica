#pragma once

#include <vector>
#include <map>
#include <memory>

#include "Camera.h"
#include "GameObject.h"
#include "Shader.h"
#include "Light.h"
#include "Water.h"
#include "ShadowMapBuffer.h"

#include <GLFW/glfw3.h>
#include "PostProcessor.h"
#include "SkyBox.h"
#include "DirectionalLight.h"

class GameInstance
{
	GameInstance();
	
	bool running = true;

	std::shared_ptr<Camera> camera;
	GLFWwindow* window;

	std::vector<std::shared_ptr<GameObject>> objects;
	std::shared_ptr<Water> water;
	
	std::shared_ptr<PointLight> pointLight;
	
	std::shared_ptr<DirectionalLight> directionalLight;

	std::map<std::string, std::shared_ptr<Shader>> shaders;
	std::shared_ptr<PostProcessor> postProcessor;
	
	bool onlyFrustumObjects = false;
	bool fPressed = false;
	bool fullscreen = true;

	static float mouseLastX;
	static float mouseLastY;
	static bool firstMouse;
	static void mouse_callback(GLFWwindow* window,	double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	std::shared_ptr<SkyBox> skyBox;
	glm::vec3 clearColor;

public:
	static GameInstance& getInstance();
	std::shared_ptr<ShadowMapBuffer> shadowMapBuffer;

	GameInstance(GameInstance const&) = delete;
	void operator=(GameInstance const&) = delete;

	void addGameObject(std::shared_ptr<GameObject> gameObject);
	void addShader(std::string name, std::shared_ptr<Shader> shader);
	void addLight(std::shared_ptr<PointLight> light);
	void addLight(std::shared_ptr<DirectionalLight> light);
	void addSkyBox(std::shared_ptr<SkyBox> skyBox);
	void setShadowMapBuffer(std::shared_ptr<ShadowMapBuffer> shadowMapBuffer);

	void setupMouse();
	void setCamera(std::shared_ptr<Camera> camera);
	std::shared_ptr<Camera> getCamera();
	void setWindow(GLFWwindow* window);

	std::shared_ptr<PointLight> getPointLight();
	std::shared_ptr<Shader> getShader(std::string name);

	void processInput(double deltaTime);
	void update(double deltaTime);
	void render();
	void render(GameObject* excludeFromRendering, glm::vec4 clipPlane);
	void render_withShader(std::shared_ptr<Shader> shader);
	void renderOclussion();
	void renderShadowMap();
	void setPostProcessor();

	void removeFullscreen();
	void setFullscreen();
	
	bool isRunning();

	void updateScreenSize(glm::ivec2 size);
};

