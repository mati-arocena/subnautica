#pragma once

#include <vector>
#include <map>
#include <memory>

#include "Camera.h"
#include "GameObject.h"
#include "Shader.h"
#include "Light.h"

#include <GLFW/glfw3.h>

class GameInstance
{
	GameInstance() {};
	
	bool running = true;

	std::shared_ptr<Camera> camera;

	GLFWwindow* window;

	std::vector<std::shared_ptr<GameObject>> objects;
	std::shared_ptr<Light> light;

	std::map<std::string, std::shared_ptr<Shader>> shaders;


	static float mouseLastX;
	static float mouseLastY;
	static bool firstMouse;
	static void mouse_callback(GLFWwindow* window,	double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	
public:
	static GameInstance& getInstance();

	GameInstance(GameInstance const&) = delete;
	void operator=(GameInstance const&) = delete;

	void addGameObject(std::shared_ptr<GameObject> gameObject);
	void addShader(std::string name, std::shared_ptr<Shader> shader);
	void addLight(std::shared_ptr<Light> light);

	void setupMouse();

	void setCamera(std::shared_ptr<Camera> camera);
	void setWindow(GLFWwindow* window);

	std::shared_ptr<Shader> getShader(std::string name);

	void processInput(double deltaTime);
	void update(double deltaTime);
	void render();
	void render(GameObject* excludeFromRendering);

	bool isRunning();
};

