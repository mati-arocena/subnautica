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
#include "Player.h"
#include "TextureLoader.h"

class GameInstance
{
	GameInstance();
	
	bool running = true;

	std::shared_ptr<Camera> camera;
	GLFWwindow* window;

	std::vector<std::shared_ptr<GameObject>> objects;

	std::vector<std::shared_ptr<Mesh>> collisionObjects;
	std::vector<std::shared_ptr<Mesh>> playerMeshes;

	std::vector<std::shared_ptr<Mesh>> particleMeshes;

	std::shared_ptr<Water> water;
	
	std::shared_ptr<Light> light;
	std::shared_ptr<Player> player;

	std::shared_ptr<DirectionalLight> directionalLight;

	std::map<std::string, std::shared_ptr<Shader>> shaders;
	std::shared_ptr<PostProcessor> postProcessor;
	
	bool fPressed = false;
	bool lPressed = false;
	bool cPressed = false;
	bool f11Pressed = false;
	bool mPressed = false;

	bool fullscreen = false;
	bool renderFrustum = false;
	bool renderWireframe = false;
	bool renderAABB = false;

	static float mouseLastX;
	static float mouseLastY;
	static bool firstMouse;
	static void mouse_callback(GLFWwindow* window,	double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	

	std::shared_ptr<SkyBox> skyBox;
	glm::vec3 clearColor;

	TextureLoader textureLoader;

public:
	static GameInstance& getInstance();
	std::shared_ptr<ShadowMapBuffer> shadowMapBuffer;

	GameInstance(GameInstance const&) = delete;
	void operator=(GameInstance const&) = delete;

	void addGameObject(std::shared_ptr<GameObject> gameObject);
	void addShader(const std::string& name, std::shared_ptr<Shader> shader);
	void addLight(std::shared_ptr<Light> light);
	void addLight(std::shared_ptr<DirectionalLight> light);
	void addSkyBox(std::shared_ptr<SkyBox> skyBox);
	void setPlayer(std::shared_ptr<Player> player);

	void setupMouse();
	void setCamera(std::shared_ptr<Camera> camera);
	std::shared_ptr<Camera> getCamera();
	void setWindow(GLFWwindow* window);
	void setShadowMapBuffer(std::shared_ptr<ShadowMapBuffer> shadowMapBuffer);

	std::shared_ptr<Light> getPointLight();
	std::shared_ptr<Shader> getShader(const std::string& name);

	std::shared_ptr<Player> getPlayer();
	glm::vec3 getMaxCollisionDelta();

	void processInput(double deltaTime);
	void update(double deltaTime);
	void render();
	void render(const GameObject* excludeFromRendering, const glm::vec4& clipPlane);
	void render_withShader(std::shared_ptr<Shader> shader);
	void renderOclussion();
	void renderShadowMap();
	void setPostProcessor();

	void removeFullscreen();
	void setFullscreen();
	
	bool isRunning() const;

	void updateScreenSize(const glm::ivec2& size);

	inline const TextureLoader& getTextureLoader() { return textureLoader; }

	bool isCollision(Movement movement);
};

