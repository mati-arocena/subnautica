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

#if 0
    std::vector<unsigned int> indices = {
        /*nbl*/0, /*nbr*/1, /*ntr*/3, /*ntl*/2, //near plane
        /*fbl*/4, /*fbr*/5, /*ftr*/7, /*ftl*/6, //far plane
        /*nbr*/1, /*ntr*/3, /*ftr*/7, /*fbr*/5, //right plane
        /*nbl*/0, /*ntl*/2, /*ftl*/6, /*fbl*/4, //left plane
        /*ntr*/3, /*ntl*/2, /*ftl*/6, /*ftr*/7, //top plane
        /*nbr*/1, /*nbl*/0, /*fbl*/4, /*fbr*/5 //bottom plane
    };
    /*fbl*/4, /*fbr*/5, /*ftr*/7, //far plane inf
        /*fbl*/4, /*ftl*/6, /*ftr*/7, //far plane sup


    /*ntl*/2, /*ntr*/3, /*ftr*/7, //top plane inf
        /*ntl*/2, /*ftl*/6, /*ftr*/7, //top plane sup

        /*nbl*/0, /*fbl*/4, /*fbr*/5, //bottom plane inf
        /*nbl*/0, /*nbr*/1, /*fbr*/5, //bottom plane sup

        /*fbl*/4, /*nbl*/0, /*ntl*/2, //left plane inf
        /*fbl*/4, /*ftl*/6, /*ntl*/2, //left plane sup

        /*fbr*/5, /*nbr*/1, /*ntr*/3, //right plane inf
        /*fbr*/5, /*ftr*/7, /*ntr*/3, //right plane sup


#endif

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

    vbo = std::make_unique<VBO>();
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

void Camera::updateViewFrustum()
{
    if (!shouldFrustumUpdate)
        return;
    
    const float halfVSide = far * std::tanf(glm::radians(Zoom) * .5f);
    const float halfHSide = halfVSide * (width / height);
    const glm::vec3 frontMultFar = far * Front;

    Plane nearFace   = { Position + near *  Front, Front};
    Plane farFace    = { Position + frontMultFar, -Front };
    Plane rightFace  = { Position, glm::cross(Up,frontMultFar + Right * halfHSide) };
    Plane leftFace   = { Position, glm::cross(frontMultFar - Right * halfHSide, Up) };
    Plane topFace    = { Position, glm::cross(Right, frontMultFar - Up * halfVSide) };
    Plane bottomFace = { Position, glm::cross(frontMultFar + Up * halfVSide, Right) };

    frustumLOD0->nearFace = nearFace;
    frustumLOD0->farFace = farFace;
    frustumLOD0->rightFace = rightFace;
    frustumLOD0->leftFace = leftFace;
    frustumLOD0->topFace = topFace;
    frustumLOD0->bottomFace = bottomFace;

    frustumLOD1->nearFace = nearFace;
    frustumLOD1->farFace = { Position + frontMultFar, -Front };
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

#if 0
    
    glm::mat4 m = ViewMatrix;
    Plane leftPlane   = { glm::vec4(m[3][0] + m[0][0], m[3][1] + m[0][1], m[3][2] + m[0][2], m[3][3] + m[0][3]) };
    Plane rightPlane  = { glm::vec4(m[3][0] - m[0][0], m[3][1] - m[0][1], m[3][2] - m[0][2], m[3][3] - m[0][3]) };
    Plane topPlane    = { glm::vec4(m[3][0] - m[1][0], m[3][1] - m[1][1], m[3][2] - m[1][2], m[3][3] - m[1][3]) };
    Plane bottomPlane = { glm::vec4(m[3][0] + m[1][0], m[3][1] + m[1][1], m[3][2] + m[1][2], m[3][3] + m[1][3]) };
    Plane nearPlane   = { glm::vec4(m[3][0] + m[2][0], m[3][1] + m[2][1], m[3][2] + m[2][2], m[3][3] + m[2][3]) };
    Plane farPlane    = { glm::vec4(m[3][0] - m[2][0], m[3][1] - m[2][1], m[3][2] - m[2][2], m[3][3] - m[2][3]) };

    frustumLOD0->leftFace = std::make_shared<Plane>(leftPlane);
    frustumLOD0->rightFace = std::make_shared<Plane>(rightPlane);
    frustumLOD0->topFace = std::make_shared<Plane>(topPlane);
    frustumLOD0->bottomFace = std::make_shared<Plane>(bottomPlane);
    frustumLOD0->nearFace = std::make_shared<Plane>(nearPlane);
    frustumLOD0->farFace = std::make_shared<Plane>(farPlane);

    frustumLOD1->leftFace = std::make_shared<Plane>(leftPlane);
    frustumLOD1->rightFace = std::make_shared<Plane>(rightPlane);
    frustumLOD1->topFace = std::make_shared<Plane>(topPlane);
    frustumLOD1->bottomFace = std::make_shared<Plane>(bottomPlane);
    frustumLOD1->nearFace = std::make_shared<Plane>(nearPlane);
    frustumLOD1->farFace = std::make_shared<Plane>(farPlane);
    
    frustumLOD2->leftFace = std::make_shared<Plane>(leftPlane);
    frustumLOD2->rightFace = std::make_shared<Plane>(rightPlane);
    frustumLOD2->topFace = std::make_shared<Plane>(topPlane);
    frustumLOD2->bottomFace = std::make_shared<Plane>(bottomPlane);
    frustumLOD2->nearFace = std::make_shared<Plane>(nearPlane);
    frustumLOD2->farFace = std::make_shared<Plane>(farPlane);
#endif
}

void Camera::updateViewMatrix()
{
    ViewMatrix = glm::lookAt(Position, Position + Front, Up);
    if (shouldFrustumUpdate)
        frustumModel = ViewMatrix;
}

void Camera::renderFrustum()
{
    if (shouldFrustumUpdate)
        return;
          
    frustumShader->use();

    frustumShader->setFloat("color", 1.f, 0.f, 0.f);
    frustumShader->setMat4("model", frustumModel);

    glBindVertexArray(vao);
    //glDrawArrays(GL_LINES, 0, indicesSize); 
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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

void Camera::changeSize(glm::ivec2 size)
{
    this->width  = size.x;
    this->height = size.y;
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
#if 0
    glm::mat3x4 A = {
        a.normal.x , a.normal.y, a.normal.z, a.distance,
        b.normal.x , b.normal.y, b.normal.z, b.distance,
        c.normal.x , c.normal.y, c.normal.z, c.distance
    };

    /* Forward elimination */
    for (int k = 0; k < 3; k++)
    {
        // Initialize maximum value and index for pivot
        int i_max = k;
        int v_max = A[i_max][k];

        /* find greater amplitude for pivot if any */
        for (int i = k + 1; i < 3; i++)
            if (abs(A[i][k]) > v_max)
                v_max = A[i][k], i_max = i;

        /* if a principal diagonal element  is zero,
         * it denotes that matrix is singular, and
         * will lead to a division-by-zero later. */

        if (!A[k][i_max])
            return glm::vec3(); // Matrix is singular

        /* Swap the greatest value row with current row */
        if (i_max != k)
            Plane::swapRows(A, k, i_max);


        for (int i = k + 1; i < 3; i++)
        {
            /* factor f to set current row kth element to 0,
             * and subsequently remaining kth column to 0 */
            double f = A[i][k] / A[k][k];

            /* subtract fth multiple of corresponding kth
               row element*/
            for (int j = k + 1; j <= 3; j++)
                A[i][j] -= A[k][j] * f;

            /* filling lower triangular matrix with zeros*/
            A[i][k] = 0;
        }

    }

    /* Backward sustitution */
    glm::vec3 x;

    /* Start calculating from last equation up to the
       first */
    for (int i = 3 - 1; i >= 0; i--)
    {
        /* start with the RHS of the equation */
        x[i] = A[i][3];

        /* Initialize j to i+1 since matrix is upper
           triangular*/
        for (int j = i + 1; j < 3; j++)
        {
            /* subtract all the lhs values
             * except the coefficient of the variable
             * whose value is being calculated */
            x[i] -= A[i][j] * x[j];
        }

        /* divide the RHS by the coefficient of the
           unknown being calculated */
        x[i] = x[i] / A[i][i];
    }

    return x;
#endif
}

void Plane::swapRows(glm::mat3x4& A, int i, int j)
{

    for (int k = 0; k <= 3; k++)
    {
        double temp = A[i][k];
        A[i][k] = A[j][k];
        A[j][k] = temp;
    }
}
