#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <vector>
#include "VBO.h"


class Frustum
{
// https://gist.github.com/podgorskiy/e698d18879588ada9014768e3e82a644
public:
    Frustum(glm::mat4 projection, glm::mat4 view);
    void update(glm::mat4 projection, glm::mat4 view);

    // http://iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm
    bool IsBoxVisible(const glm::vec3& minp, const glm::vec3& maxp) const;

    void renderDebug();
    glm::vec3* getPoint();
private:
    enum Planes
    {
        Left = 0,
        Right,
        Bottom,
        Top,
        Near,
        Far,
        Count,
        Combinations = Count * (Count - 1) / 2
    };

    template<Planes i, Planes j>
    struct ij2k
    {
        enum { k = i * (9 - i) / 2 + j - 1 };
    };

    template<Planes a, Planes b, Planes c>
    glm::vec3 intersection(const glm::vec3* crosses) const;

    glm::vec4   m_planes[Count];
    glm::vec3   m_points[8];

    std::shared_ptr<class Shader> frustumShader;
    glm::mat4 frustumModel;
    unsigned int vao, ebo, indicesSize;
    VBO* vbo;

    void setupDebug();
};


template<Frustum::Planes a, Frustum::Planes b, Frustum::Planes c>
inline glm::vec3 Frustum::intersection(const glm::vec3* crosses) const
{
    float D = glm::dot(glm::vec3(m_planes[a]), crosses[ij2k<b, c>::k]);
    glm::vec3 res = glm::mat3(crosses[ij2k<b, c>::k], -crosses[ij2k<a, c>::k], crosses[ij2k<a, b>::k]) *
        glm::vec3(m_planes[a].w, m_planes[b].w, m_planes[c].w);
    return res * (-1.0f / D);
}

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
    float height = 480;
    float width = 640;
    float near = 0.1f;
    float far = 200.f;
    std::shared_ptr<Frustum> frustumLOD0;
    std::shared_ptr<Frustum> frustumLOD1;
    std::shared_ptr<Frustum> frustumLOD2;
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
    void SetPosition(glm::vec3 position);
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

	void changeSize(glm::ivec2 size);

    std::shared_ptr<Frustum> getFrustum(enum class LOD lod);
    void createViewFrustum();

    void renderFrustum();
    void toggleFrustumUpdate();
private:

    bool shouldFrustumUpdate = true;

    void updateFrustums();
    void updateCameraVectors();
};
