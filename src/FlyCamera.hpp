#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum class Direction {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};


class FlyCamera {
private:

    const float cameraSensitivity = 0.1f;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float cameraSpeed {10.0f};
    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 200.0f;
    glm::vec3 cameraPos {glm::vec3(0.0f, 0.0f, 10.0f)};
    glm::vec3 cameraFront {glm::vec3(0.0f, 0.0f, -1.0)};
    glm::vec3 cameraRight {glm::vec3(0.0f, 0.0f, 0.0f)};
    glm::vec3 worldUp {glm::vec3(0.0f, 1.0f, 0.0f)};
    glm::mat4 view = glm::mat4(1.0f);

public:
    FlyCamera() {}

    void updatePosition(Direction dir, float deltaTime);
    void updateFront(float xOffset, float yOffset);
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRation) const;
    glm::vec3 getCameraPosition() const {return cameraPos;};

};

