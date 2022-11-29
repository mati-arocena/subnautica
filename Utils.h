#pragma once

#include <glm/glm.hpp>
#include <assimp/types.h>

// Converts an assimp 4x4 matrix to a glm matrix
static inline glm::mat4 convertMatrix(const aiMatrix4x4 &assMap)
{
	return {
		assMap.a1, assMap.b1, assMap.c1, assMap.d1,
		assMap.a2, assMap.b2, assMap.c2, assMap.d2,
		assMap.a3, assMap.b3, assMap.c3, assMap.d3,
		assMap.a4, assMap.b4, assMap.c4, assMap.d4,
	};
}

static inline glm::quat toQuat(const aiQuaternion& pOrientation)
{
	return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
}

static inline glm::vec3 toVec3(const aiVector3D& vec)
{
	return glm::vec3(vec.x, vec.y, vec.z);
}

static inline glm::vec3 toVec3(const aiColor3D& vec)
{
	return {
		vec.r, vec.g, vec.b
	};
}