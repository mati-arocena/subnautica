#pragma once

#include <memory>
#include <vector>

#include "Camera.h"

class Shader;


class GameObject
{

public:
	virtual void render() = 0;
	virtual void update(double deltaTime) = 0;
	virtual void render_withShader(std::shared_ptr<Shader> shader) = 0;
	virtual void renderOclussion() = 0;

};

