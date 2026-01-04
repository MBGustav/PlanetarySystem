#pragma once



#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(glm::vec3 startPos);
    void processInput(void* window, float deltaTime);
    glm::mat4 getViewMatrix() const;
    glm::mat4 perspective() const;
    glm::mat4 lookAt() const;

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
};



Camera::Camera(glm::vec3 startPos)
    : position(startPos), front(glm::vec3(0.0f, 0.0f, -1.0f)), up(glm::vec3(0.0f, 1.0f, 0.0f)) {}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::perspective() const {
    return glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
}

glm::mat4 Camera::lookAt() const {
    return glm::lookAt(position, position + front, up);
}
void Camera::processInput(void* window, float deltaTime) {
    float cameraSpeed = 2.5f * deltaTime;
    // Input handling logic goes here
    // Example:
    // if (key W is pressed)
    //     position += cameraSpeed * front;
    // if (key S is pressed)
    //     position -= cameraSpeed * front;
    // if (key A is pressed)
    //     position -= glm::normalize(glm::cross(front, up)) * cameraSpeed;
    // if (key D is pressed)
    //     position += glm::normalize(glm::cross(front, up)) * cameraSpeed;
}