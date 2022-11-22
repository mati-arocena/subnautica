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

void resizeWindow(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main()
{
	GameInstance& gameInstance = GameInstance::getInstance();
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(640, 480, "Subnautica", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	gameInstance.setWindow(window);

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

	gameInstance.addLight(std::make_shared<Light>(glm::vec3{1.f, 1.f, 1.f}, glm::vec3{0.f, 10.f, 0.f}));
	
	gameInstance.addGameObject(std::make_shared<Model>("assets/mar2.gltf"));
	gameInstance.addGameObject(std::make_shared<Water>());
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