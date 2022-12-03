#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Frustum.h"
#include <memory>
#include <vector>
#include "VBO.h"
#include "ConfigManager.h"


// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


class Camera
{
    float height;
    float width;
    float near; 
    float far;

    void updateCameraVectors();
    
    std::shared_ptr<Frustum> frustumLOD0;
    std::shared_ptr<Frustum> frustumLOD1;
    std::shared_ptr<Frustum> frustumLOD2;

    inline void createFrustums();
    inline void updateFrustumsVectors();
    inline void updateFrustumsPosition();
    inline void updateFrustumsZoom();

public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
	

    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    // constructor with vectors
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW, float pitch = PITCH
    );
    // constructor with scalar values
    Camera(
        float posX, float posY, float posZ,
        float upX, float upY, float upZ,
        float yaw, float pitch
    );
    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() const;
    void SetPosition(const glm::vec3& position);
    void InvertPitch();
    glm::mat4 GetProjectionMatrix() const;
    glm::vec4 GetPosition() const; 
    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);
    void updateViewMatrix();

	void changeSize(const glm::ivec2& size);

    std::shared_ptr<Frustum> getFrustum(enum class LOD lod);
    void toggleFrustumUpdate();
    void renderFrustum();
};

void Camera::updateFrustumsVectors()
{
    frustumLOD0->update(Front, Right, Up, width / height);
    frustumLOD1->update(Front, Right, Up, width / height);
    frustumLOD2->update(Front, Right, Up, width / height);
}

void Camera::updateFrustumsPosition()
{
    frustumLOD0->update(Position);
    frustumLOD1->update(Position);
    frustumLOD2->update(Position);
}

void Camera::updateFrustumsZoom()
{
    frustumLOD0->update(glm::radians(Zoom), width / height, Right);
    frustumLOD1->update(glm::radians(Zoom), width / height, Right);
    frustumLOD2->update(glm::radians(Zoom), width / height, Right);
}

void Camera::createFrustums()
{
    frustumLOD0 = std::make_shared<Frustum>(Position, Front, Up, Right, glm::radians(Zoom), near, far * 0.68, width / height);
    frustumLOD1 = std::make_shared<Frustum>(Position, Front, Up, Right, glm::radians(Zoom), far * 0.68, far * 0.95, width / height);
    frustumLOD2 = std::make_shared<Frustum>(Position, Front, Up, Right, glm::radians(Zoom), far * 0.95, far, width / height);
}
