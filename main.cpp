#include <GL/glew.h>
#include <GL/GLU.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include "Camera.h"
#include "VBO.h"
#include "Shader.h"
#include "Model.h"
#include "Definitions.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>
#include "Water.h"
#include "UIRenderer.h"
#include "GameInstance.h"

#include "btBulletDynamicsCommon.h"
#include "SkyBox.h"
#include "ConfigManager.h"
#include <thread>

void resizeWindow(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	ConfigManager::getInstance().setWindowSize(glm::ivec2(width, height));
	GameInstance::getInstance().updateScreenSize(glm::ivec2(width, height));
}

int main()
{
	GameInstance& gameInstance = GameInstance::getInstance();
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	
	unsigned width, height;
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	width = mode->width;
	height = mode->height;
	ConfigManager::getInstance().setWindowSize(glm::ivec2(width, height));
	
	width = ConfigManager::getInstance().getWindowSize().x;
	height = ConfigManager::getInstance().getWindowSize().y;
	GLFWwindow* window = glfwCreateWindow(width, height, "Subnautica", monitor, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	gameInstance.setWindow(window);

	//glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		return -1;
	std::shared_ptr<Camera> camera = std::make_shared<Camera>();
	gameInstance.setCamera(camera);


	glfwSetFramebufferSizeCallback(window, resizeWindow);
	gameInstance.setupMouse();



	gameInstance.addShader(NORMAL_SHADER, std::make_shared<Shader>(NORMAL_SHADER + ".vert", NORMAL_SHADER + ".frag"));
	gameInstance.addShader(UI_SHADER, std::make_shared<Shader>(UI_SHADER + ".vert", UI_SHADER + ".frag"));
	gameInstance.addShader(WATER_SHADER, std::make_shared<Shader>(WATER_SHADER + ".vert", WATER_SHADER + ".frag"));
	gameInstance.addShader(OCCLUSION_SHADER, std::make_shared<Shader>(NORMAL_SHADER + ".vert", OCCLUSION_SHADER + ".frag"));
	gameInstance.addShader(WATER_SHADER_OCCLUSION, std::make_shared<Shader>(NORMAL_SHADER + ".vert", WATER_SHADER_OCCLUSION + ".frag"));
	gameInstance.addShader(POST_SHADER, std::make_shared<Shader>(POST_SHADER + ".vert", POST_SHADER + ".frag"));
	gameInstance.addShader(SKY_BOX_SHADER, std::make_shared<Shader>(SKY_BOX_SHADER + ".vert", SKY_BOX_SHADER + ".frag"));
	gameInstance.addShader(SHADOW_MAP_SHADER, std::make_shared<Shader>(SHADOW_MAP_SHADER + ".vert", SHADOW_MAP_SHADER + ".frag"));

	std::shared_ptr<ShadowMapBuffer> shadowMapBuffer = std::make_shared<ShadowMapBuffer>(gameInstance.getShader(SHADOW_MAP_SHADER));

	gameInstance.addLight(std::make_shared<PointLight>(glm::vec3{1.f, 1.f, 1.f}, glm::vec3{1000.f, 1000.f, 0.f}));
	gameInstance.setShadowMapBuffer(shadowMapBuffer);
	
	gameInstance.addGameObject(std::make_shared<Model>("assets/mar2.gltf"));
	gameInstance.addGameObject(std::make_shared<Water>());
	gameInstance.addSkyBox(std::make_shared<SkyBox>());
	gameInstance.setPostProcessor();

	UIRenderer gui(gameInstance.getShader(UI_SHADER));

	glEnable(GL_DEPTH_TEST);
	auto lastTime = std::chrono::system_clock::now();
	while (gameInstance.isRunning())
	{
		auto current = std::chrono::system_clock::now();

		std::chrono::duration<double> elapsed = current - lastTime;
		gameInstance.processInput(elapsed.count());
		
		gameInstance.update(elapsed.count());
		gameInstance.render();

		glfwSwapBuffers(window);
		glfwPollEvents();

		
		lastTime = current;
	}

	glfwTerminate();
	return 0;
}