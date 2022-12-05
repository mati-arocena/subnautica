#include "Player.h"
#include <GLFW/glfw3.h>
#include "GameInstance.h"

void Player::move(const glm::vec3& movement, double deltaTime)
{
	for (auto& mesh : meshesLOD0)
	{
		mesh->move(movement * static_cast<float>(deltaTime));
	}
	for (auto& mesh : meshesLOD1)
	{
		mesh->move(movement * static_cast<float>(deltaTime));
	}
	for (auto& mesh : meshesLOD2)
	{
		mesh->move(movement * static_cast<float>(deltaTime));
	}
}

void Player::rotate(float angle, double deltaTime)
{
	for (auto& mesh : meshesLOD0)
	{
		mesh->rotate(rotationAxis, angle * static_cast<float>(deltaTime));
	}
	for (auto& mesh : meshesLOD1)
	{
		mesh->rotate(rotationAxis, angle * static_cast<float>(deltaTime));
	}
	for (auto& mesh : meshesLOD2)
	{
		mesh->rotate(rotationAxis, angle * static_cast<float>(deltaTime));
	}
}

void Player::render()
{
	if (GameInstance::getInstance().getCamera()->getMode() == PLAYER_MODE)
	{
		for (auto& mesh : meshesLOD0)
		{
			mesh->render();
		}

		for (ParticleGenerator& particleGenerator : particleGenerators)
		{
			particleGenerator.render();
		}
	}
	else
	{
		Model::render();
	}
}

void Player::update(double deltaTime)
{
	Model::update(deltaTime);

	rotate(movementAngle, deltaTime);

	position += movementVector;

	if (movementVector.x != 0.f || movementVector.y != 0.f || movementVector.z != 0.f)
	{
		move(movementVector, deltaTime);
	}

	//for (ParticleGenerator& particleGenerator : particleGenerators)
	//{
	//	particleGenerator.move(movementVector, deltaTime, getFront());
	//	particleGenerator.setVelocity(-getFront() * forwardVelocity);
	//}

	movementAngle = 0.f;
	movementVector = { 0.f, 0.f, 0.f };
}

void Player::move(Movement movement)
{
	glm::mat4 rot = glm::toMat4(meshesLOD0[0]->rotation);
	switch (movement)
	{
	case Movement::UP:
		if (position.y < 0.f)
			movementVector += glm::vec3{ 0.f, upVelocity, 0.f };
		break;
	case Movement::DOWN:
		if (position.y > -1300.f)
			movementVector += glm::vec3{ 0.f, -upVelocity, 0.f };
		break;
	case Movement::LEFT:
		movementAngle += rotationVelocity;
		break;
	case Movement::RIGHT:
		movementAngle -= rotationVelocity;
		break;
	case Movement::FORWARD:
		movementVector += glm::vec3(glm::normalize(rot * front)) * forwardVelocity;
		break;
	case Movement::BACKWARD:
		movementVector -= glm::vec3(glm::normalize(rot * front)) * forwardVelocity;
		break;
	default:
		break;
	}

	const glm::vec3& collisionDelta = GameInstance::getInstance().getMaxCollisionDelta();
	if (glm::length(collisionDelta) > 0.01f)
	{

		move(collisionDelta);
	}

}

void Player::move(const glm::vec3& movement)
{
	for (auto& mesh : meshesLOD0)
	{
		mesh->move(movement);
	}
	for (auto& mesh : meshesLOD1)
	{
		mesh->move(movement);
	}
	for (auto& mesh : meshesLOD2)
	{
		mesh->move(movement);
	}
}

float Player::getYaw()
{
	return glm::eulerAngles( meshesLOD0[0]->rotation).y;
}

glm::vec3 Player::getFront()
{
	glm::mat4 rot = glm::toMat4(meshesLOD0[0]->rotation);
	return glm::vec3(glm::normalize(rot * front));
}

glm::vec3 Player::getPosition() const
{
	return meshesLOD0[0]->translation;
}
