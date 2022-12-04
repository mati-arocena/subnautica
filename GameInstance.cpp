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

void GameInstance::setShadowMapBuffer(std::shared_ptr<ShadowMapBuffer> shadowMapBuffer)
{
	this->shadowMapBuffer = shadowMapBuffer;
}

void GameInstance::addShader(const std::string& name, std::shared_ptr<Shader> shader)
{
	shaders[name] = std::move(shader);
}

void GameInstance::setupMouse()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Disable mouse
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
}

void GameInstance::addLight(std::shared_ptr<Light> light)
{
	this->light = std::move(light);
}

void GameInstance::addLight(std::shared_ptr<DirectionalLight> light)
{
	this->directionalLight = std::move(light);
}

void GameInstance::addSkyBox(std::shared_ptr<SkyBox> skyBox)
{
	this->skyBox = std::move(skyBox);
}

void GameInstance::setPlayer(std::shared_ptr<Player> player)
{
	this->player = std::move(player);
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
		mouseLastX = static_cast<float>(xpos);
		mouseLastY = static_cast<float>(ypos);
		firstMouse = false;
	}

	float xoffset = static_cast<float>(xpos - mouseLastX);
	float yoffset = static_cast<float>(mouseLastY - ypos); // reversed since y-coordinates range from bottom to top
	mouseLastX = static_cast<float>(xpos);
	mouseLastY = static_cast<float>(ypos);
	getInstance().camera->ProcessMouseMovement(xoffset, yoffset);
}

void GameInstance::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	getInstance().camera->ProcessMouseScroll(static_cast<float>(yoffset));
}

std::shared_ptr<Camera> GameInstance::getCamera() 
{
	return this->camera;
}

void GameInstance::setCamera(std::shared_ptr<Camera> camera)
{
	this->camera = std::move(camera);
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
		glfwSetWindowShouldClose(window, static_cast<int>(true));
		running = false;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->ProcessKeyboard(FORWARD, static_cast<float>(deltaTime));
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->ProcessKeyboard(BACKWARD, static_cast<float>(deltaTime));
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->ProcessKeyboard(LEFT, static_cast<float>(deltaTime));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->ProcessKeyboard(RIGHT, static_cast<float>(deltaTime));

	// F11 is fullscreen
	if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS)
	{
		f11Pressed = true;
	}

	
	// F11 is fullscreen
	if (f11Pressed && glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS)
	{
		f11Pressed = false;
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
		f11Pressed = false;
	}
	
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		fPressed = true;
	if (fPressed && glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
	{
		fPressed = false;
		camera->toggleFrustumUpdate();
		renderFrustum = !renderFrustum;
	}

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		lPressed = true;
	if (lPressed && glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE)
	{
		lPressed = false;
		renderWireframe = !renderWireframe;
	}

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		cPressed = true;
	if (cPressed && glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE)
	{
		cPressed = false;
		renderAABB = !renderAABB;
	}
	
	
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		mPressed = true;
	if (mPressed && glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE)
	{
		mPressed = false;
		Mode mode = camera->getMode();
		if (mode == PLAYER_MODE)
			camera->setMode(FLY_MODE);
		else
			camera->setMode(PLAYER_MODE);
	}
	
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		player->move(Movement::FORWARD);
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		player->move(Movement::BACKWARD);
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		player->move(Movement::RIGHT);
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		player->move(Movement::LEFT);
	}
	
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
	{
		player->move(Movement::UP);
	}

	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
	{
		player->move(Movement::DOWN);
	}
}


void GameInstance::update(double deltaTime)
{
	camera->updateViewMatrix();

	clearColor = ConfigManager::getInstance().getClearColor();
	//TODO: light->update(deltaTime);
	player->update(deltaTime);

	for (auto& object : objects)
	{
		object->update(deltaTime);
	}

	light->updatePosition(camera->GetPosition());

	particleGenerator->update(deltaTime);
}

std::shared_ptr<Shader> GameInstance::getShader(const std::string& name)
{
	if (shaders.find(name) == shaders.end()) {
		return std::shared_ptr<Shader>();
	}
	else {
		return shaders[name];
	}
}

void GameInstance::render(const GameObject* excludeFromRendering, const glm::vec4& clipPlane)
{
	glClearColor(BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, BACKGROUND_COLOR.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto& shader : shaders)
	{
		shader.second->prerender(camera, light);
	}

	glDisable(GL_DEPTH_TEST);
	if (skyBox)
	{
		skyBox->render();
	}
	glEnable(GL_DEPTH_TEST);


	for (auto& object : objects)
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

void GameInstance::renderShadowMap()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	shadowMapBuffer->bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	shadowMapBuffer->shader->lightSpaceTransform(this->light);
	for (auto object : objects)
	{
		if (dynamic_cast<Model*>(object.get()))
		{
			object->render_withShader(shadowMapBuffer->shader);
		}
	}

	player->render_withShader(shadowMapBuffer->shader);

	shadowMapBuffer->unbind();

	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);

}


void GameInstance::setParticleGenerator()
{
	this->particleGenerator = std::make_shared<ParticleGenerator>(glm::vec3(.0f, .0f, .0f));
}

void GameInstance::render()
{
	renderShadowMap();

	// Render escena
	glm::ivec2 windowSize = ConfigManager::getInstance().getWindowSize();
	glBindTexture(GL_TEXTURE_2D, 0);
	glViewport(0, 0, windowSize.x, windowSize.y);
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessor->getSceneFB());

	glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto shader : shaders)
	{
		shader.second->prerender(camera, light);
	}

	glDisable(GL_DEPTH_TEST);
	if (skyBox)
	{
		skyBox->render();
	}
	glEnable(GL_DEPTH_TEST);

	if (renderAABB) 
	{
		player->renderAABB();
		for (auto& object : objects)
		{
			object->renderAABB();
		}
	}
	if (renderWireframe)
	{
		player->renderWireframe();
		for (auto& object : objects)
		{
			object->renderWireframe();
		}
	}
	else
	{
		player->render();
		for (auto& object : objects)
		{
			object->render();
		}
	}

	if (renderFrustum)
		camera->renderFrustum();

	particleGenerator->render();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	postProcessor->draw();
}

bool GameInstance::isRunning() const
{
	return running;
}

void GameInstance::updateScreenSize(const glm::ivec2& size)
{
	this->camera->changeSize(size);
}

std::shared_ptr<Player> GameInstance::getPlayer()
{
	return player;
}

std::shared_ptr<Light> GameInstance::getPointLight() {
	return this->light;
}

void GameInstance::render_withShader(std::shared_ptr<Shader> shader)
{
	// Renders a scene with the same shader
	glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader->prerender(camera, light);
	
	glDisable(GL_DEPTH_TEST);
	if (skyBox)
	{
		skyBox->render();
	}
	glEnable(GL_DEPTH_TEST);

	for (auto object : objects)
	{
		if (dynamic_cast<Model*>(object.get()))
		{
			object->render_withShader(shader);
		}
	}
}

void GameInstance::renderOclussion()
{
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	std::shared_ptr<Shader> occShdr = getShader(OCCLUSION_SHADER);
	occShdr->prerender(camera, light);


	if (water != NULL)
	{
		this->water->renderOclussion();
	}
	player->render_withShader(occShdr);
	for (auto object : objects)
	{
		if (dynamic_cast<Model*>(object.get()))
		{
			object->render_withShader(occShdr);
		}
	}
}


void GameInstance::setPostProcessor()
{
	this->postProcessor = std::make_unique<PostProcessor>();
}
