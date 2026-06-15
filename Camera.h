
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;

    float yaw;
    float pitch;
    float speed;
    float sensitivity;

    Camera();

    void processKeyboard(const float deltaTime, bool forward, bool backward, bool right, bool left);
    void processMouse(float offsetX, float offsetY);
    glm::mat4 getView();
};