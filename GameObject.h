#pragma once

#include <memory>
#include <vector>

#include "Camera.h"
#include "Light.h"

class GameObject
{
public:
	virtual void update(double deltaTime) = 0;
	virtual void render() = 0;
};

