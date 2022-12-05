#include "Camera.h"
#include "ConfigManager.h"
#include "Mesh.h"
#include "Definitions.h"
#include "GameInstance.h"
#include "Shader.h"

Camera::Camera(
    glm::vec3 position,
    glm::vec3 up,
    float yaw, float pitch
) : Position{ position }, 
    Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
    WorldUp {up}, 
    Yaw {yaw}, 
    Pitch {pitch},
    MovementSpeed(SPEED), 
    MouseSensitivity(SENSITIVITY), 
    Zoom(ZOOM)  
{
    mode = FLY_MODE;
    auto& config = ConfigManager::getInstance();
    height = static_cast<float>(config.getWindowSize().x);
    width = static_cast<float>(config.getWindowSize().y);
    near = config.getNear();
    far = config.getFar();

    updateCameraVectors();
    createFrustums();

	float aspect = (float)width / (float)height;
    ProjectionMatrix = glm::perspective(glm::radians(Zoom), aspect, near, far);
}

Camera::Camera(
    float posX, float posY, float posZ,
    float upX, float upY, float upZ,
    float yaw, float pitch
) : Position{posX, posY, posZ},Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
    WorldUp{upX, upY, upZ},
    MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{

    mode = FLY_MODE;
    Yaw = yaw;
    Pitch = pitch;

    auto& config = ConfigManager::getInstance();
    height = static_cast<float>(config.getWindowSize().x);
    width = static_cast<float>(config.getWindowSize().y);
    near = config.getNear();
    far = config.getFar();
    
    updateCameraVectors();
    createFrustums();

    float aspect = float(width) / float(height);
    ProjectionMatrix = glm::perspective(glm::radians(Zoom), aspect, near, far);

}

void Camera::updateViewMatrix()
{
    if (mode == PLAYER_MODE)
    {
        glm::vec4 playerFront = glm::vec4(GameInstance::getInstance().getPlayer()->getFront(), 1.0f);
        Front = playerFront;
        Position = GameInstance::getInstance().getPlayer()->getPosition() - Front * .8f + glm::vec3 (0.f, 2.f, 0.f);
        if(!isDoingReflection)
            updateFrustumsPosition();
    }
    ViewMatrix = glm::lookAt(Position, Position + Front, Up);        
}

std::shared_ptr<Frustum> Camera::getFrustum(LOD lod)
{
    if (lod == LOD::LOD0)
    {
        return frustumLOD0;
    }
    else if (lod == LOD::LOD1)
    {
        return frustumLOD1;
    }
    else
    {
        return frustumLOD2;
    }
}

void Camera::changeSize(const glm::ivec2& size)
{
    this->width  = static_cast<float>(size.x);
    this->height = static_cast<float>(size.y);
    ProjectionMatrix = glm::perspective(glm::radians(Zoom), width / height, near, far);
    if(!isDoingReflection)
        updateFrustumsVectors();
}

void Camera::toggleFrustumUpdate()
{
    frustumLOD0->toggleFreeze();
    frustumLOD1->toggleFreeze();
    frustumLOD2->toggleFreeze();
}

void Camera::renderFrustum()
{
    frustumLOD0->render({ 0.f, 1.f, 0.f });
    frustumLOD1->render({ 0.f, 1.f, 1.f });
    frustumLOD2->render({ 1.f, 1.f, 0.f });
}

void Camera::setMode(Mode mode)
{
    this->mode = mode;
}


glm::mat4 Camera::GetViewMatrix() const
{
    return ViewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    return ProjectionMatrix;
}

glm::vec4 Camera::GetPosition() const
{
    return glm::vec4(Position, 1.f);
}

void Camera::SetPosition(const glm::vec3& position) 
{
    this->Position = position;
    if(!isDoingReflection)
        updateFrustumsPosition();
}

void Camera::InvertPitch()
{
    this->Pitch = -Pitch;
    updateCameraVectors();
    if(!isDoingReflection)  
        updateFrustumsVectors();
}


void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    if (mode == PLAYER_MODE)
        return;
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
    if(!isDoingReflection)
        updateFrustumsPosition();
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    if (mode == PLAYER_MODE)
        return;
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw = glm::mod(Yaw + xoffset, 360.0f);
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
    if (!isDoingReflection)
       updateFrustumsVectors();

}

void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 90.0f)
        Zoom = 90.0f;
	float aspect = float(width) / float(height);
	
    ProjectionMatrix = glm::perspective(glm::radians(Zoom), aspect, 0.1f, 100.0f);

    if(!isDoingReflection)
        updateFrustumsZoom();
}

void Camera::updateCameraVectors()
{
    
    if (mode == PLAYER_MODE)
    {
        Front = glm::vec4(GameInstance::getInstance().getPlayer()->getFront(), 1.0f);

        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));

        if (!isDoingReflection)
            updateFrustumsVectors();
    }
    else
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));

    }
    

} 

Mode Camera::getMode() const
{
    return mode;
}

void Camera::startReflections(float posY)
{
    isDoingReflection = true;
    this->Position.y = posY;
    this->InvertPitch();
    this->updateViewMatrix();
}

void Camera::stopReflections(glm::vec3 pos)
{
    this->InvertPitch();
    this->SetPosition(pos);
    this->updateViewMatrix();
    isDoingReflection = false;
}
