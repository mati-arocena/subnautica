#pragma once
#include "Model.h"

#include "ConfigManager.h"

enum class Movement {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	FORWARD,
	BACKWARD
};

class Player : public Model
{
	const glm::vec3 rotationAxis = { 0.f, 0.f, -1.f };
	const glm::vec4 front = { 0.f, -1.f, 0.f, 1.0f };
	const float forwardVelocity = 2.5f;
	const float upVelocity = 1.f;
	const float rotationVelocity = 1.f;
	

	glm::vec3 movementVector = { 0.f, 0.f, 0.f };
	float movementAngle = 0.f;
	
	bool firstPerson;

	void move(const glm::vec3& movement, double deltaTime);
	void rotate(float angle, double deltaTime);
public:
	Player(const std::string& path, const std::string& extension) : Model(path, extension, glm::vec3(0.f, 0.f, 0.f)) {
		this->isMovable = true;
		firstPerson = ConfigManager::getInstance().getMode() == PLAYER_MODE;
	};

	Player(const std::string& path, const std::string& extension, const std::string& animationPath, const std::string& animationExtension) : Model(path, extension, animationPath, animationExtension, glm::vec3(0.f)) {
		firstPerson = ConfigManager::getInstance().getMode() == PLAYER_MODE;
		this->isMovable = true;
	};

	void render() override;
	void update(double deltaTime) override;

	void move(Movement movement);
	void move(const glm::vec3& movement);

	float getYaw();
	glm::vec3 getFront();
	glm::vec3 getPosition() const;
};

