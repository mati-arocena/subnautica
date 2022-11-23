#include "Camera.h"
#include "btBulletDynamicsCommon.h"

Camera::Camera(
    glm::vec3 position,
    glm::vec3 up,
    float yaw, float pitch
) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    createViewFrustum(640, 480, 0.1f, 100.f);
    updateCameraVectors();
    ProjectionMatrix = glm::perspective(glm::radians(Zoom), 640.0f / 480.0f, 0.1f, 200.0f);

}

Camera::Camera(
    float posX, float posY, float posZ,
    float upX, float upY, float upZ,
    float yaw, float pitch
) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    createViewFrustum(640, 480, 0.1f, 100.f);
    updateCameraVectors();
    ProjectionMatrix = glm::perspective(glm::radians(Zoom), 640.0f / 480.0f, 0.1f, 200.0f);
}


void Camera::createViewFrustum(float screenWidth, float screenHeight, float near, float far)
{
    const btScalar nearPlane = near;
    const btScalar farPlane = far;

    const btScalar planesFraction = farPlane / nearPlane;
    const btScalar centralPlate = (farPlane - nearPlane) * 0.5;

    const btScalar aspect = (btScalar)screenWidth / (btScalar)screenHeight;

    bool isScreenWide = screenWidth > screenHeight;
    const btScalar left = isScreenWide ? -aspect :  1.0;
    const btScalar right = isScreenWide ? aspect : -1.0;
    const btScalar bottom = isScreenWide ? -1.0 : -aspect;
    const btScalar top = isScreenWide ? 1.0 : aspect;
    const btScalar farLeft = left * planesFraction;
    const btScalar farRight = right * planesFraction;
    const btScalar farBottom = bottom * planesFraction;
    const btScalar farTop = top * planesFraction;

    btConvexHullShape* shape = new btConvexHullShape();

    shape->addPoint(btVector3(left, top, centralPlate));
    shape->addPoint(btVector3(right, top, centralPlate));
    shape->addPoint(btVector3(left, bottom, centralPlate));
    shape->addPoint(btVector3(right, bottom, centralPlate));
    shape->addPoint(btVector3(farLeft, farTop, centralPlate));
    shape->addPoint(btVector3(farRight, farTop, centralPlate));
    shape->addPoint(btVector3(farLeft, farBottom, centralPlate));
    shape->addPoint(btVector3(farRight, farBottom, centralPlate));

    btCompoundShape* frustumShape = new btCompoundShape();
    const btVector3 v(0., 0., -(nearPlane + farPlane));
    const btQuaternion q = btQuaternion::getIdentity();
    btTransform T(q, v);
    frustumShape->addChildShape(T, shape);

    frustum = std::make_shared<btPairCachingGhostObject>();
    frustum->setCollisionShape(frustumShape);
    frustum->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
}

void Camera::updateViewMatrix()
{
    ViewMatrix = glm::lookAt(Position, Position + Front, Up);
}

std::shared_ptr<btPairCachingGhostObject> Camera::getViewFrustum() const
{
    return frustum;
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

void Camera::SetPosition(glm::vec3 position) 
{
    this->Position = position;
}

void Camera::InvertPitch()
{
    this->Pitch = -Pitch;
    updateCameraVectors();
}


void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
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
}

void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 90.0f)
        Zoom = 90.0f;
    ProjectionMatrix = glm::perspective(glm::radians(Zoom), 640.0f / 480.0f, 0.1f, 100.0f);
}


void Camera::updateCameraVectors()
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

    glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMatrix);
    
    btTransform T;
    T.setFromOpenGLMatrix(modelViewMatrix);
    frustum->setWorldTransform(T.inverse());
}