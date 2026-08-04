#include "FlyCamera.hpp"

#include <algorithm>

// FlyCamera::FlyCamera() {}

void FlyCamera::updatePosition(Direction dir, float deltaTime) {
    switch(dir) {
        case Direction::FORWARD:
            cameraPos += cameraSpeed * deltaTime * cameraFront;
            break;
        case Direction::BACKWARD:
            cameraPos -= cameraSpeed * deltaTime *  cameraFront;
            break;
        case Direction::LEFT:
            cameraPos -= cameraSpeed * deltaTime * cameraRight;
            break;
        case Direction::RIGHT:
            cameraPos += cameraSpeed * deltaTime * cameraRight;
    }
}

void FlyCamera::updateFront(float xOffset, float yOffset) {
    xOffset *= cameraSensitivity;
    yOffset *= cameraSensitivity;
    yaw += xOffset;
    pitch += yOffset;

    pitch = std::clamp(pitch, -89.0f, 89.0f);

    cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
    cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)); 
    cameraFront.y = sin(glm::radians(pitch));
    cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(cameraFront);
}

glm::mat4 FlyCamera::getViewMatrix() const {
    return glm::lookAt(
        cameraPos,
        cameraPos + cameraFront,
        worldUp
    );
}

glm::mat4 FlyCamera::getProjectionMatrix(float aspectRation) const {
    return glm::perspective(
        glm::radians(fov), 
        aspectRation,
        nearPlane,
        farPlane
    );
}