#include "Frustum.h"

#include "Mesh.h"
#include "Definitions.h"
#include "GameInstance.h"

Frustum::Frustum(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up, const glm::vec3& right, float fov, float near, float far, float aspect)
{
	this->near = near;
	this->far = far;
	this->fov = fov;

	update(position, front, up, right, aspect);
	setupRender();
}

void Frustum::update(const glm::vec3& position, const glm::vec3& front, const glm::vec3& right, const glm::vec3& up, float aspect)
{
	if (freeze)
		return;
	update(position);
	update(front, right, up, aspect);
}

void Frustum::update(const glm::vec3& front, const glm::vec3& right, const glm::vec3& up, float aspect)
{
	if (freeze)
		return;
	this->front = front;
	this->up = up;

	/* http://www.lighthouse3d.com/tutorials/view-frustum-culling/view-frustums-shape/ */
	float heightNear = 2 * tan(fov * 0.5f) * near;
	float widthNear = heightNear * aspect;

	float heightFar = 2 * tan(fov * 0.5f) * far;
	float widthFar = heightFar * aspect;

	frontMultNear = front * near;
	frontMultFar = front * far;

	/* https://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-extracting-the-planes */
	farTopLeftSum = (up * heightFar * 0.5f) - (right * widthFar * 0.5f);
	farTopRightSum = (up * heightFar * 0.5f) + (right * widthFar * 0.5f);
	farBottomLeftSum = -(up * heightFar * 0.5f) - (right * widthFar * 0.5f);
	farBottomRightSum = -(up * heightFar * 0.5f) + (right * widthFar * 0.5f);

	nearTopLeftSum = (up * heightNear * 0.5f) - (right * widthNear * 0.5f);
	nearTopRightSum = (up * heightNear * 0.5f) + (right * widthNear * 0.5f);
	nearBottomLeftSum = -(up * heightNear * 0.5f) - (right * widthNear * 0.5f);
	nearBottomRightSum = -(up * heightNear * 0.5f) + (right * widthNear * 0.5f);
}

void Frustum::update(const glm::vec3& position)
{
	if (freeze)
		return;
	this->position = position;
}

void Frustum::update(float fov, float aspect, glm::vec3 right)
{
	if (freeze)
		return;
	this->fov = fov;
	update(position, front, right, up, aspect);
}


FrustumVertices Frustum::calculateVertices() const
{
	glm::vec3 farCenter = position + frontMultFar;
	glm::vec3 nearCenter = position + frontMultNear;

	FrustumVertices result{};

	result.farTopLeft = farCenter + farTopLeftSum;
	result.farTopRight = farCenter + farTopRightSum;
	result.farBottomLeft = farCenter + farBottomLeftSum;
	result.farBottomRight = farCenter + farBottomRightSum;

	result.nearTopLeft = nearCenter + nearTopLeftSum;
	result.nearTopRight = nearCenter + nearTopRightSum;
	result.nearBottomLeft = nearCenter + nearBottomLeftSum;
	result.nearBottomRight = nearCenter + nearBottomRightSum;

	return result;
}

FrustumVertices Frustum::toWorldCoordinates(const FrustumVertices& vertices)
{
	FrustumVertices result{};

	result.farTopLeft = toWorldCoordinates(result.farTopLeft);
	result.farTopRight = toWorldCoordinates(result.farTopRight);
	result.farBottomLeft = toWorldCoordinates(result.farBottomLeft);
	result.farBottomRight = toWorldCoordinates(result.farBottomRight);

	result.nearTopLeft = toWorldCoordinates(result.nearTopLeft);
	result.nearTopRight = toWorldCoordinates(result.nearTopRight);
	result.nearBottomLeft = toWorldCoordinates(result.nearBottomLeft);
	result.nearBottomRight = toWorldCoordinates(result.nearBottomRight);

	return result;
}

void Frustum::setupRender()
{
	frustumShader = GameInstance::getInstance().getShader(FRUSTUM_SHADER);

	const FrustumVertices frustumVertices = calculateVertices();

	Vertex nbl(frustumVertices.nearBottomLeft, { 0.f, 1.f, 0.f }, { 0.f, 1.f });
	Vertex nbr(frustumVertices.nearBottomRight, { 0.f, 1.f, 0.f }, { 0.f, 1.f });
	Vertex ntl(frustumVertices.nearTopLeft, { 0.f, 1.f, 0.f }, { 0.f, 1.f });
	Vertex ntr(frustumVertices.nearTopRight, { 0.f, 1.f, 0.f }, { 0.f, 1.f });
	Vertex fbl(frustumVertices.farBottomLeft, { 0.f, 1.f, 0.f }, { 0.f, 1.f });
	Vertex fbr(frustumVertices.farBottomRight, { 0.f, 1.f, 0.f }, { 0.f, 1.f });
	Vertex ftl(frustumVertices.farTopLeft, { 0.f, 1.f, 0.f }, { 0.f, 1.f });
	Vertex ftr(frustumVertices.farTopRight, { 0.f, 1.f, 0.f }, { 0.f, 1.f });

	std::vector<Vertex> vertices = {
		nbl/*0*/, nbr/*1*/, ntl/*2*/, ntr/*3*/,
		fbl/*4*/, fbr/*5*/, ftl/*6*/, ftr/*7*/
	};

	std::vector<unsigned int> indices = {
		//near plane
		0, 1, 1, 3, 3, 2, 2, 0,
		//far plane
		4, 5, 5, 7, 7, 6, 6, 4,
		//bottom plane
		0, 4, 4, 5, 5, 1, 1, 0,
		//top plane
		2, 3, 3, 7, 7, 6, 6, 2

	};
	indicesSize = static_cast<unsigned int>(indices.size());

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	this->vbo = std::make_shared<VBO>();
	vbo->load(vertices, vertices.size() * Vertex::numElementsInVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), VBO::toEBO(indices), GL_STATIC_DRAW);

	Vertex::setVertexAttributes();
}

void Frustum::render(const glm::vec3& color /*= { 0.f, 1.f, 0.f }*/)
{
	frustumShader->use();

	frustumShader->setFloat("color", color.r, color.g, color.b);

	glm::mat4 model = getModelMatrix();
	frustumShader->setMat4("model", model);

	glBindVertexArray(vao);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_LINES, indicesSize, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

bool Frustum::isBoxInFrustum(const glm::vec3& minp, const glm::vec3& maxp) const
{
	
#if 0
	// check box outside/inside of frustum
	if ((glm::dot(topFace.equation(), glm::vec4(minp.x, minp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(topFace.equation(), glm::vec4(maxp.x, minp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(topFace.equation(), glm::vec4(minp.x, maxp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(topFace.equation(), glm::vec4(maxp.x, maxp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(topFace.equation(), glm::vec4(minp.x, minp.y, maxp.z, 1.0f)) > 0.4) &&
		(glm::dot(topFace.equation(), glm::vec4(maxp.x, minp.y, maxp.z, 1.0f)) > 0.4) &&
		(glm::dot(topFace.equation(), glm::vec4(minp.x, maxp.y, maxp.z, 1.0f)) > 0.4) &&
		(glm::dot(topFace.equation(), glm::vec4(maxp.x, maxp.y, maxp.z, 1.0f)) > 0.4))
	{
		return false;
	}
	if ((glm::dot(bottomFace.equation(), glm::vec4(minp.x, minp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(bottomFace.equation(), glm::vec4(maxp.x, minp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(bottomFace.equation(), glm::vec4(minp.x, maxp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(bottomFace.equation(), glm::vec4(maxp.x, maxp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(bottomFace.equation(), glm::vec4(minp.x, minp.y, maxp.z, 1.0f)) > 0.4) &&
		(glm::dot(bottomFace.equation(), glm::vec4(maxp.x, minp.y, maxp.z, 1.0f)) > 0.4) &&
		(glm::dot(bottomFace.equation(), glm::vec4(minp.x, maxp.y, maxp.z, 1.0f)) > 0.4) &&
		(glm::dot(bottomFace.equation(), glm::vec4(maxp.x, maxp.y, maxp.z, 1.0f)) > 0.4))
	{
		return false;
	}
	if ((glm::dot(rightFace.equation(), glm::vec4(minp.x, minp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(rightFace.equation(), glm::vec4(maxp.x, minp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(rightFace.equation(), glm::vec4(minp.x, maxp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(rightFace.equation(), glm::vec4(maxp.x, maxp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(rightFace.equation(), glm::vec4(minp.x, minp.y, maxp.z, 1.0f)) > 0.4) &&
		(glm::dot(rightFace.equation(), glm::vec4(maxp.x, minp.y, maxp.z, 1.0f)) > 0.4) &&
		(glm::dot(rightFace.equation(), glm::vec4(minp.x, maxp.y, maxp.z, 1.0f)) > 0.4) &&
		(glm::dot(rightFace.equation(), glm::vec4(maxp.x, maxp.y, maxp.z, 1.0f)) > 0.4))
	{
		return false;
	}
	if ((glm::dot(leftFace.equation(), glm::vec4(minp.x, minp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(leftFace.equation(), glm::vec4(maxp.x, minp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(leftFace.equation(), glm::vec4(minp.x, maxp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(leftFace.equation(), glm::vec4(maxp.x, maxp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(leftFace.equation(), glm::vec4(minp.x, minp.y, maxp.z, 1.0f)) > 0.4) &&
		(glm::dot(leftFace.equation(), glm::vec4(maxp.x, minp.y, maxp.z, 1.0f)) > 0.4) &&
		(glm::dot(leftFace.equation(), glm::vec4(minp.x, maxp.y, maxp.z, 1.0f)) > 0.4) &&
		(glm::dot(leftFace.equation(), glm::vec4(maxp.x, maxp.y, maxp.z, 1.0f)) > 0.4))
	{
		return false;
	}
	if ((glm::dot(farFace.equation(), glm::vec4(minp.x, minp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(farFace.equation(), glm::vec4(maxp.x, minp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(farFace.equation(), glm::vec4(minp.x, maxp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(farFace.equation(), glm::vec4(maxp.x, maxp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(farFace.equation(), glm::vec4(minp.x, minp.y, maxp.z, 1.0f)) > 0.4) &&
		(glm::dot(farFace.equation(), glm::vec4(maxp.x, minp.y, maxp.z, 1.0f)) > 0.4) &&
		(glm::dot(farFace.equation(), glm::vec4(minp.x, maxp.y, maxp.z, 1.0f)) > 0.4) &&
		(glm::dot(farFace.equation(), glm::vec4(maxp.x, maxp.y, maxp.z, 1.0f)) > 0.4))
	{
		return false;
	}
	if ((glm::dot(nearFace.equation(), glm::vec4(minp.x, minp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(nearFace.equation(), glm::vec4(maxp.x, minp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(nearFace.equation(), glm::vec4(minp.x, maxp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(nearFace.equation(), glm::vec4(maxp.x, maxp.y, minp.z, 1.0f)) > 0.4) &&
		(glm::dot(nearFace.equation(), glm::vec4(minp.x, minp.y, maxp.z, 1.0f)) > 0.4) &&
		(glm::dot(nearFace.equation(), glm::vec4(maxp.x, minp.y, maxp.z, 1.0f)) > 0.4) &&
		(glm::dot(nearFace.equation(), glm::vec4(minp.x, maxp.y, maxp.z, 1.0f)) > 0.4) &&
		(glm::dot(nearFace.equation(), glm::vec4(maxp.x, maxp.y, maxp.z, 1.0f)) > 0.4))
	{
		return false;
	}


	// check frustum outside/inside box
	int out;
	out = 0; for (int i = 0; i < 8; i++) out += ((points[i].x > maxp.x) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((points[i].x < minp.x) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((points[i].y > maxp.y) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((points[i].y < minp.y) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((points[i].z > maxp.z) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((points[i].z < minp.z) ? 1 : 0); if (out == 8) return false;

#endif
	return true;
}

void Frustum::toggleFreeze()
{
	freeze = !freeze;
}
