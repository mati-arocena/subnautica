#pragma once
#include "Model.h"

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
	const glm::vec3 rotationAxis = { 0.f, 0.f, 1.f };
	const float forwardVelocity = 1.f;
	const float upVelocity = 1.f;
	const float rotationVelocity = 1.f;
	
	glm::vec3 position = { 0.f, 0.f, 0.f };
	float orientation = 0.f;

	glm::vec3 movementVector = { 0.f, 0.f, 0.f };
	float movementAngle = 0.f;
	
	void move(const glm::vec3& movement, double deltaTime);
	void rotate(float angle, double deltaTime);
public:
	Player(std::string path, std::string extension) : Model(path, extension) {};
	Player(std::string path, std::string extension, std::string animationPath, std::string animationExtension) : Model(path, extension, animationPath, animationExtension) {};

	void update(double deltaTime) override;

	void move(Movement movement);
};

