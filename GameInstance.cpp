#include "GameInstance.h"
#include <GLFW/glfw3.h>
#include "Model.h"
#include "Definitions.h"
#include "Water.h"
#include "ConfigManager.h"

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
	auto waterPtr = std::dynamic_pointer_cast<Water>(gameObject);
	if (waterPtr)
	{
		water = waterPtr;
	}
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

void GameInstance::addLight(std::shared_ptr<PointLight> light)
{
	pointLight = light;
}

void GameInstance::addLight(std::shared_ptr<DirectionalLight> light)
{
	this->directionalLight = light;
}

void GameInstance::addSkyBox(std::shared_ptr<SkyBox> skyBox)
{
	this->skyBox = skyBox;
}


GameInstance::GameInstance()
{
	this->clearColor = ConfigManager::getInstance().getClearColor();
	this->window = nullptr;
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

void GameInstance::removeFullscreen()
{
	glfwSetWindowMonitor(window, NULL, 100, 100, 800, 600, 0);
}

void GameInstance::setFullscreen()
{
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
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
	// F11 is fullscreen
	if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS)
	{
		if (!fullscreen)
		{
			setFullscreen();
			fullscreen = true;
		}
		else
		{
			removeFullscreen();
			fullscreen = false;
		}
	}
	
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		fPressed = true;
	if (fPressed && glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
	{
		fPressed = false;
		onlyFrustumObjects = !onlyFrustumObjects;
	}
}

void GameInstance::update(double deltaTime)
{
	camera->updateViewMatrix();

	clearColor = ConfigManager::getInstance().getClearColor();
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
	glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto shader : shaders)
	{
		shader.second->prerender(camera, pointLight);
	}

	glDisable(GL_DEPTH_TEST);
	if (skyBox)
	{
		skyBox->render();
	}
	glEnable(GL_DEPTH_TEST);

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
	glm::ivec2 windowSize = ConfigManager::getInstance().getWindowSize();
	glBindTexture(GL_TEXTURE_2D, 0);
	glViewport(0, 0, windowSize.x, windowSize.y);
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessor->getSceneFB());

	glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto shader : shaders)
	{
		shader.second->prerender(camera, pointLight);
	}

	glDisable(GL_DEPTH_TEST);
	if (skyBox)
	{
		skyBox->render();
	}
	glEnable(GL_DEPTH_TEST);
	
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

void GameInstance::updateScreenSize(glm::ivec2 size)
{
	this->camera->changeSize(size);
}

std::shared_ptr<PointLight> GameInstance::getPointLight() {
	return this->pointLight;
}

void GameInstance::render_withShader(std::shared_ptr<Shader> shader)
{
	// Renders a scene with the same shader
	glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader->prerender(camera, pointLight);
	
	glDisable(GL_DEPTH_TEST);
	if (skyBox)
	{
		skyBox->render();
	}
	glEnable(GL_DEPTH_TEST);

	for (auto object : objects)
	{
		if (auto m = dynamic_cast<Model*>(object.get()))
		{
			object->render_withShader(shader);
		}
	}
}

void GameInstance::renderOclussion()
{
	glClearColor(1.f, 1.f, 1.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	std::shared_ptr<Shader> occShdr = getShader(OCCLUSION_SHADER);
	occShdr->prerender(camera, pointLight);

	if (water != NULL)
	{
		this->water->renderOclussion();
	}
	for (auto object : objects)
	{
		if (auto m = dynamic_cast<Model*>(object.get()))
		{
			object->render_withShader(occShdr);
		}
	}
}


void GameInstance::setPostProcessor()
{
	this->postProcessor = std::make_unique<PostProcessor>();
}
