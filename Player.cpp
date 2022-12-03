#include "Player.h"
#include <GLFW/glfw3.h>

void Player::move(const glm::vec3& movement, double deltaTime)
{
	for (auto& mesh : meshesLOD0)
	{
		mesh.move(movement * static_cast<float>(deltaTime));
	}
	for (auto& mesh : meshesLOD1)
	{
		mesh.move(movement * static_cast<float>(deltaTime));
	}
	for (auto& mesh : meshesLOD2)
	{
		mesh.move(movement * static_cast<float>(deltaTime));
	}
}

void Player::rotate(float angle, double deltaTime)
{


	for (auto& mesh : meshesLOD0)
	{
		mesh.rotate(rotationAxis, angle * static_cast<float>(deltaTime));
	}
	for (auto& mesh : meshesLOD1)
	{
		mesh.rotate(rotationAxis, angle * static_cast<float>(deltaTime));
	}
	for (auto& mesh : meshesLOD2)
	{
		mesh.rotate(rotationAxis, angle * static_cast<float>(deltaTime));
	}
}

void Player::update(double deltaTime)
{
	Model::update(deltaTime);
	rotate(movementAngle, deltaTime);
	movementAngle = 0.f;

	position += movementVector;
	if (movementVector.x != 0.f || movementVector.y != 0.f || movementVector.z != 0.f)
	{
		move(movementVector, deltaTime);
	}
	movementVector = { 0.f, 0.f, 0.f };
}

void Player::move(Movement mov)
{
	switch (mov)
	{
	case Movement::UP:
		if (position.z < 0.f)
			movementVector += glm::vec3{ 0.f, 0.f, upVelocity };
		break;
	case Movement::DOWN:
		movementVector += glm::vec3{ 0.f, 0.f, -upVelocity };
		break;
	case Movement::LEFT:
		movementAngle += rotationVelocity;
		break;
	case Movement::RIGHT:
		movementAngle -= rotationVelocity;
		break;
	case Movement::FORWARD:
		movementVector += glm::vec3{ 0.f, -forwardVelocity, 0.f };
		break;
	case Movement::BACKWARD:
		movementVector += glm::vec3{ 0.f, forwardVelocity, 0.f };
		break;
	default:
		break;
	}

}
