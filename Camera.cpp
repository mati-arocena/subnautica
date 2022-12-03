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
) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;

    updateCameraVectors();
	float aspect = (float)width / (float)height;
    ProjectionMatrix = glm::perspective(glm::radians(Zoom), aspect, near, far);

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

	float aspect = float(width) / float(height);
    ProjectionMatrix = glm::perspective(glm::radians(Zoom), aspect, near, far);
}

void Camera::createViewFrustum()
{
    shouldFrustumUpdate = true;

    frustumLOD0 = std::make_shared<Frustum>();
    frustumLOD1 = std::make_shared<Frustum>();
    frustumLOD2 = std::make_shared<Frustum>();

    updateViewFrustum();

    frustumShader = GameInstance::getInstance().getShader(FRUSTUM_SHADER);

    Frustum f = *frustumLOD0; //frustum to debug

    glm::vec3 nearBottomLeft = Plane::intersectPlanes(f.nearFace, f.bottomFace, f.leftFace);
    glm::vec3 nearBottomRight = Plane::intersectPlanes(f.nearFace, f.bottomFace, f.rightFace);
    glm::vec3 nearTopLeft = Plane::intersectPlanes(f.nearFace, f.topFace, f.leftFace);
    glm::vec3 nearTopRight = Plane::intersectPlanes(f.nearFace, f.topFace, f.rightFace);
    glm::vec3 farBottomLeft = Plane::intersectPlanes(f.farFace, f.bottomFace, f.leftFace);
    glm::vec3 farBottomRight = Plane::intersectPlanes(f.farFace, f.bottomFace, f.rightFace);
    glm::vec3 farTopLeft = Plane::intersectPlanes(f.farFace, f.topFace, f.leftFace);
    glm::vec3 farTopRight = Plane::intersectPlanes(f.farFace, f.topFace, f.rightFace);

    Vertex nbl(nearBottomLeft, nearBottomLeft, { 0.f, 1.f });
    Vertex nbr(nearBottomRight, nearBottomRight, { 0.f, 1.f });
    Vertex ntl(nearTopLeft, nearTopLeft, { 0.f, 1.f });
    Vertex ntr(nearTopRight, nearTopRight, { 0.f, 1.f });
    Vertex fbl(farBottomLeft, farBottomLeft, { 0.f, 1.f });
    Vertex fbr(farBottomRight, farBottomRight, { 0.f, 1.f });
    Vertex ftl(farTopLeft, farTopLeft, { 0.f, 1.f });
    Vertex ftr(farTopRight, farTopRight, { 0.f, 1.f });
  
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

    vbo = std::make_unique<VBO>();
    vbo->load(vertices, vertices.size() * Vertex::numElementsInVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), VBO::toEBO(indices), GL_STATIC_DRAW);

    Vertex::setVertexAttributes();
}

void Camera::updateViewFrustum()
{
    if (!shouldFrustumUpdate)
        return;
    
    const float halfVSide = far * std::tanf(glm::radians(Zoom) * .5f);
    const float halfHSide = halfVSide * (width / height);
    const glm::vec3 frontMultFar = far * Front;
    //const glm::vec3 frontMultFar = 10.f * Front;

    Plane nearFace   = { Position + near *  Front, Front};
    Plane farFace    = { Position + frontMultFar, -Front };
    Plane rightFace  = { Position, glm::cross(Up,frontMultFar + Right * halfHSide) };
    Plane leftFace   = { Position, glm::cross(frontMultFar - Right * halfHSide, Up) };
    Plane topFace    = { Position, glm::cross(Right, frontMultFar - Up * halfVSide) };
    Plane bottomFace = { Position, glm::cross(frontMultFar + Up * halfVSide, Right) };
   // Plane bottomFace = { Position, glm::cross(Right, frontMultFar + Up * halfVSide) };

    glm::vec3 nearBottomLeft = Plane::intersectPlanes(nearFace, bottomFace, leftFace);
    glm::vec3 nearBottomRight = Plane::intersectPlanes(nearFace, bottomFace, rightFace);
    glm::vec3 nearTopLeft = Plane::intersectPlanes(nearFace, topFace, leftFace);
    glm::vec3 nearTopRight = Plane::intersectPlanes(nearFace, topFace, rightFace);
    glm::vec3 farBottomLeft = Plane::intersectPlanes(farFace, bottomFace, leftFace);
    glm::vec3 farBottomRight = Plane::intersectPlanes(farFace, bottomFace, rightFace);
    glm::vec3 farTopLeft = Plane::intersectPlanes(farFace, topFace, leftFace);
    glm::vec3 farTopRight = Plane::intersectPlanes(farFace, topFace, rightFace);

    frustumLOD0->nearFace = nearFace;
    frustumLOD0->farFace = { Position + frontMultFar * .68f, -Front };
    frustumLOD0->rightFace = rightFace;
    frustumLOD0->leftFace = leftFace;
    frustumLOD0->topFace = topFace;
    frustumLOD0->bottomFace = bottomFace;

    frustumLOD0->points[0] = nearBottomLeft;
    frustumLOD0->points[1] = nearBottomRight;
    frustumLOD0->points[2] = nearTopLeft;
    frustumLOD0->points[3] = nearTopRight;
    frustumLOD0->points[4] = farBottomLeft;
    frustumLOD0->points[5] = farBottomRight;
    frustumLOD0->points[6] = farTopLeft;
    frustumLOD0->points[7] = farTopRight;

    frustumLOD1->nearFace = nearFace;
    frustumLOD1->farFace = { Position + frontMultFar * .95f, -Front };
    frustumLOD1->rightFace = rightFace;
    frustumLOD1->leftFace = leftFace;
    frustumLOD1->topFace = topFace;
    frustumLOD1->bottomFace = bottomFace;

    frustumLOD2->nearFace = nearFace;
    frustumLOD2->farFace = { Position + frontMultFar, -Front };
    frustumLOD2->rightFace = rightFace;
    frustumLOD2->leftFace = leftFace;
    frustumLOD2->topFace = topFace;
    frustumLOD2->bottomFace = bottomFace;

}

void Camera::updateViewMatrix()
{
	if (mode == Mode::FLY_MODE)
	{
        ViewMatrix = glm::lookAt(Position, Position + Front, Up);
	}
	
		
    ViewMatrix = glm::lookAt(Position, Position + Front, Up);
    if (shouldFrustumUpdate) 
    {
        frustumModel = glm::inverse(ProjectionMatrix * ViewMatrix);
    }
        
}

void Camera::renderFrustum()
{
    if (shouldFrustumUpdate)
        return;
          
    frustumShader->use();

    frustumShader->setFloat("color", 0.f, 1.f, 0.f);
    frustumShader->setMat4("model", frustumModel);

    glBindVertexArray(vao);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_LINES, indicesSize, GL_UNSIGNED_INT, 0); 
    glBindVertexArray(0);
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

void Camera::changeSize(const glm::ivec2& size)
{
    this->width  = static_cast<float>(size.x);
    this->height = static_cast<float>(size.y);
    ProjectionMatrix = glm::perspective(glm::radians(Zoom), width / height, near, far);
    updateViewFrustum();
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
	float aspect = float(width) / float(height);
	
    ProjectionMatrix = glm::perspective(glm::radians(Zoom), aspect, 0.1f, 100.0f);

    updateViewFrustum();

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
    
    updateViewFrustum();
} 

glm::vec3 Plane::intersectPlanes(const Plane& a, const Plane& b, const Plane& c)
{
    // Formula used
    //                d1 ( N2 * N3 ) + d2 ( N3 * N1 ) + d3 ( N1 * N2 )
    //P =   ---------------------------------------------------------------------
    //                             N1 . ( N2 * N3 )
    //
    // Note: N refers to the normal, d refers to the displacement. '.' means dot product. '*' means cross product

    glm::vec3 v1, v2, v3;
    float f = -glm::dot(a.normal, glm::cross(b.normal, c.normal));

    v1 = (a.distance * (glm::cross(b.normal, c.normal)));
    v2 = (b.distance * (glm::cross(c.normal, a.normal)));
    v3 = (c.distance * (glm::cross(a.normal, b.normal)));

    glm::vec3 vec = { v1.x + v2.x + v3.x, v1.y + v2.y + v3.y, v1.z + v2.z + v3.z };
    return vec / f;
}

