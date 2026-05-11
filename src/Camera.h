#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

const GLfloat YAW        = -90.0f;
const GLfloat PITCH      =   0.0f;
const GLfloat SPEED      =   6.0f;
const GLfloat SENSITIVTY =   0.25f;
const GLfloat ZOOM       =  45.0f;

class Camera
{
public:
    glm::vec3 position, front, up, right, worldUp;
    GLfloat yaw, pitch, movementSpeed, mouseSensitivity, zoom;

    Camera(glm::vec3 pos = glm::vec3(0.0f),
           glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f),
           GLfloat yaw = YAW, GLfloat pitch = PITCH)
        : front(glm::vec3(0.0f, 0.0f, -1.0f)),
          movementSpeed(SPEED), mouseSensitivity(SENSITIVTY), zoom(ZOOM)
    {
        this->position = pos;
        this->worldUp  = worldUp;
        this->yaw      = yaw;
        this->pitch    = pitch;
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix() {
        return glm::lookAt(position, position + front, up);
    }

    void ProcessKeyboard(Camera_Movement dir, GLfloat dt) {
        GLfloat v = movementSpeed * dt;
        if (dir == FORWARD)  position += front * v;
        if (dir == BACKWARD) position -= front * v;
        if (dir == LEFT)     position -= right * v;
        if (dir == RIGHT)    position += right * v;
    }

    void ProcessMouseMovement(GLfloat xOff, GLfloat yOff, GLboolean constrainPitch = true) {
        xOff *= mouseSensitivity;
        yOff *= mouseSensitivity;
        yaw   += xOff;
        pitch += yOff;
        if (constrainPitch) {
            if (pitch >  89.0f) pitch =  89.0f;
            if (pitch < -89.0f) pitch = -89.0f;
        }
        updateCameraVectors();
    }

    GLfloat GetZoom()     { return zoom; }
    glm::vec3 GetPosition() { return position; }
    glm::vec3 GetFront()    { return front; }

private:
    void updateCameraVectors() {
        glm::vec3 f;
        f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        f.y = sin(glm::radians(pitch));
        f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(f);
        right = glm::normalize(glm::cross(front, worldUp));
        up    = glm::normalize(glm::cross(right, front));
    }
};
