#include "Camera.h"
#include "btBulletDynamicsCommon.h"
#include "ConfigManager.h"
#include "Mesh.h"
#include "Definitions.h"
#include "GameInstance.h"
#include "Shader.h"

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

    updateCameraVectors();
    ProjectionMatrix = glm::perspective(glm::radians(Zoom), width / height, near, far);
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

    
    updateCameraVectors();
    ProjectionMatrix = glm::perspective(glm::radians(Zoom), width / height , near, far);

}

void Camera::createViewFrustum()
{
    updateFrustums();
}

void Camera::updateViewMatrix()
{
    ViewMatrix = glm::lookAt(Position, Position + Front, Up);
    if (shouldFrustumUpdate)
    {
        updateFrustums();
    }
}

void Camera::renderFrustum()
{
    if (shouldFrustumUpdate)
        return;
          
    frustumLOD0->renderDebug();
}

void Camera::toggleFrustumUpdate()
{
    shouldFrustumUpdate = !shouldFrustumUpdate;
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

void Camera::changeSize(glm::ivec2 size)
{
    this->width  = size.x;
    this->height = size.y;
    ProjectionMatrix = glm::perspective(glm::radians(Zoom), width / height, near, far);
 
    if (shouldFrustumUpdate)
    {
        updateFrustums();
    }
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
	float aspect = width / height;
	
    ProjectionMatrix = glm::perspective(glm::radians(Zoom), aspect, 0.1f, 100.0f);
    
    if (shouldFrustumUpdate) 
    {
        updateFrustums();
    }
}


void Camera::updateFrustums()
{
    glm::mat4 pm0 = glm::perspective(glm::radians(Zoom), width / height, near, far * 0.68f);
    frustumLOD0 = std::make_shared<Frustum>(pm0, ViewMatrix);
    glm::mat4 pm1 = glm::perspective(glm::radians(Zoom), width / height, near, far * 0.95f);
    frustumLOD1 = std::make_shared<Frustum>(pm1, ViewMatrix);
    frustumLOD2 = std::make_shared<Frustum>(ProjectionMatrix, ViewMatrix);
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
}

Frustum::Frustum(glm::mat4 projection, glm::mat4 view)
{
    update(projection, view);
    setupDebug();
}

void Frustum::update(glm::mat4 projection, glm::mat4 view)
{
    glm::mat4 m = projection * view;
    m = glm::transpose(m);
    glm::vec4 camPosition = GameInstance::getInstance().getCamera()->GetPosition();
    frustumModel = glm::translate(m, glm::vec3(camPosition.x, camPosition.y, camPosition.z));
    m_planes[Left] = m[3] + m[0];
    m_planes[Right] = m[3] - m[0];
    m_planes[Bottom] = m[3] + m[1];
    m_planes[Top] = m[3] - m[1];
    m_planes[Near] = m[3] + m[2];
    m_planes[Far] = m[3] - m[2];

    glm::vec3 crosses[Combinations] = {
        glm::cross(glm::vec3(m_planes[Left]),   glm::vec3(m_planes[Right])),
        glm::cross(glm::vec3(m_planes[Left]),   glm::vec3(m_planes[Bottom])),
        glm::cross(glm::vec3(m_planes[Left]),   glm::vec3(m_planes[Top])),
        glm::cross(glm::vec3(m_planes[Left]),   glm::vec3(m_planes[Near])),
        glm::cross(glm::vec3(m_planes[Left]),   glm::vec3(m_planes[Far])),
        glm::cross(glm::vec3(m_planes[Right]),  glm::vec3(m_planes[Bottom])),
        glm::cross(glm::vec3(m_planes[Right]),  glm::vec3(m_planes[Top])),
        glm::cross(glm::vec3(m_planes[Right]),  glm::vec3(m_planes[Near])),
        glm::cross(glm::vec3(m_planes[Right]),  glm::vec3(m_planes[Far])),
        glm::cross(glm::vec3(m_planes[Bottom]), glm::vec3(m_planes[Top])),
        glm::cross(glm::vec3(m_planes[Bottom]), glm::vec3(m_planes[Near])),
        glm::cross(glm::vec3(m_planes[Bottom]), glm::vec3(m_planes[Far])),
        glm::cross(glm::vec3(m_planes[Top]),    glm::vec3(m_planes[Near])),
        glm::cross(glm::vec3(m_planes[Top]),    glm::vec3(m_planes[Far])),
        glm::cross(glm::vec3(m_planes[Near]),   glm::vec3(m_planes[Far]))
    };

    m_points[0] = intersection<Left, Bottom, Near>(crosses);
    m_points[1] = intersection<Left, Top, Near>(crosses);
    m_points[2] = intersection<Right, Bottom, Near>(crosses);
    m_points[3] = intersection<Right, Top, Near>(crosses);
    m_points[4] = intersection<Left, Bottom, Far>(crosses);
    m_points[5] = intersection<Left, Top, Far>(crosses);
    m_points[6] = intersection<Right, Bottom, Far>(crosses);
    m_points[7] = intersection<Right, Top, Far>(crosses);
}

bool Frustum::IsBoxVisible(const glm::vec3& minp, const glm::vec3& maxp) const
{
    // check box outside/inside of frustum
    for (int i = 0; i < Count; i++)
    {
        if ((glm::dot(m_planes[i], glm::vec4(minp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
            (glm::dot(m_planes[i], glm::vec4(maxp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
            (glm::dot(m_planes[i], glm::vec4(minp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
            (glm::dot(m_planes[i], glm::vec4(maxp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
            (glm::dot(m_planes[i], glm::vec4(minp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
            (glm::dot(m_planes[i], glm::vec4(maxp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
            (glm::dot(m_planes[i], glm::vec4(minp.x, maxp.y, maxp.z, 1.0f)) < 0.0) &&
            (glm::dot(m_planes[i], glm::vec4(maxp.x, maxp.y, maxp.z, 1.0f)) < 0.0))
        {
            return false;
        }
    }

    // check frustum outside/inside box
    int out;
    out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].x > maxp.x) ? 1 : 0); if (out == 8) return false;
    out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].x < minp.x) ? 1 : 0); if (out == 8) return false;
    out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].y > maxp.y) ? 1 : 0); if (out == 8) return false;
    out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].y < minp.y) ? 1 : 0); if (out == 8) return false;
    out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].z > maxp.z) ? 1 : 0); if (out == 8) return false;
    out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].z < minp.z) ? 1 : 0); if (out == 8) return false;

    return true;
}

void Frustum::renderDebug()
{
    frustumShader->use();

    frustumShader->setFloat("color", 0.f, 1.f, 0.f);
    frustumShader->setMat4("model", frustumModel);

    glBindVertexArray(vao);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_LINES, indicesSize, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Frustum::setupDebug()
{
    frustumShader = GameInstance::getInstance().getShader(FRUSTUM_SHADER);

    Vertex nblV(m_points[0], m_points[0], {0.f, 1.f});
    Vertex nbrV(m_points[2], m_points[2], {0.f, 1.f});
    Vertex ntlV(m_points[1], m_points[1], {0.f, 1.f});
    Vertex ntrV(m_points[3], m_points[3], {0.f, 1.f});
    Vertex fblV(m_points[4], m_points[4], {0.f, 1.f});
    Vertex fbrV(m_points[6], m_points[6], { 0.f, 1.f });
    Vertex ftlV(m_points[5], m_points[5], { 0.f, 1.f });
    Vertex ftrV(m_points[7], m_points[7], { 0.f, 1.f });

    std::vector<Vertex> vertices = {
        nblV/*0*/, nbrV/*1*/, ntlV/*2*/, ntrV/*3*/,
        fblV/*4*/, fbrV/*5*/, ftlV/*6*/, ftrV/*7*/
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
    indicesSize = indices.size();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    vbo = new VBO();
    vbo->load(Vertex::toVBO(vertices), vertices.size() * Vertex::numElementsInVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), VBO::toEBO(indices), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Vertex::numElementsInVBO * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, Vertex::numElementsInVBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, Vertex::numElementsInVBO * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, Vertex::numElementsInVBO * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, Vertex::numElementsInVBO * sizeof(float), (void*)(11 * sizeof(float)));
    glEnableVertexAttribArray(4);
}
