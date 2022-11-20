#include "GameInstance.h"
#include <GLFW/glfw3.h>
#include "Model.h"
#include "Definitions.h"

float GameInstance::mouseLastX = 400;
float GameInstance::mouseLastY = 300;
bool GameInstance::firstMouse = true;


GameInstance& GameInstance::getInstance()
{
	static GameInstance instance;
	return instance;
}

void GameInstance::addGameObject(std::shared_ptr<GameObject> gameObject)
{
	objects.push_back(gameObject);
}

void GameInstance::addShader(std::string name, std::shared_ptr<Shader> shader)
{
	shaders[name] = shader;
}

void GameInstance::setupMouse()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Disable mouse
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
}

void GameInstance::addLight(std::shared_ptr<Light> light)
{
	this->light = light;
}

void GameInstance::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) // initially set to true
	{
		mouseLastX = xpos;
		mouseLastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - mouseLastX;
	float yoffset = mouseLastY - ypos; // reversed since y-coordinates range from bottom to top
	mouseLastX = xpos;
	mouseLastY = ypos;
	getInstance().camera->ProcessMouseMovement(xoffset, yoffset);
}

void GameInstance::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	getInstance().camera->ProcessMouseScroll(yoffset);
}

std::shared_ptr<Camera> GameInstance::getCamera() 
{
	return this->camera;
}

void GameInstance::setCamera(std::shared_ptr<Camera> camera)
{
	this->camera = camera;
}

void GameInstance::setWindow(GLFWwindow* window)
{
	this->window = window;
}

void GameInstance::processInput(double deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
		running = false;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->ProcessKeyboard(RIGHT, deltaTime);
}

void GameInstance::update(double deltaTime)
{
	camera->updateViewMatrix();

	//TODO: light->update(deltaTime);

	for (auto object : objects)
	{
		object->update(deltaTime);
	}

}

std::shared_ptr<Shader> GameInstance::getShader(std::string name)
{
	if (shaders.find(name) == shaders.end()) {
		return std::shared_ptr<Shader>();
	}
	else {
		return shaders[name];
	}
}

void GameInstance::render(GameObject* excludeFromRendering, glm::vec4 clipPlane)
{
	glClearColor(BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, BACKGROUND_COLOR.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto shader : shaders)
	{

		shader.second->prerender(camera, light);
	}

	for (auto object : objects)
	{
		if (auto m = dynamic_cast<Model*>(object.get()))
		{
			m->clipModel(clipPlane);
		}

		if (object.get() != excludeFromRendering)
		{
			object->render();
		}
	}
}

void GameInstance::render()
{
	// Render escena
	glBindTexture(GL_TEXTURE_2D, 0);
	glViewport(0, 0, 800, 600);
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessor->getSceneFB());

	glClearColor(BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, BACKGROUND_COLOR.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto shader : shaders)
	{
		shader.second->prerender(camera, light);
	}

	for (auto object : objects)
	{
		object->render();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	postProcessor->draw();
}

bool GameInstance::isRunning()
{
	return running;
}

std::shared_ptr<Light> GameInstance::getLight() {
	return this->light;
}

void GameInstance::render_withShader(std::shared_ptr<Shader> shader)
{
	// Renders a scene with the same shader

	glClearColor(1.f, 1.f, 1.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader->prerender(camera, light);

	for (auto object : objects)
	{
		if (auto m = dynamic_cast<Model*>(object.get()))
		{
			object->render_withShader(shader);
		}
	}
}


void GameInstance::setPostProcessor()
{
	this->postProcessor = std::make_unique<PostProcessor>();
}
