
#include "Camera.h"

#include <cmath>

using namespace glm;

Camera::Camera() {
    position    = vec3(0.0f, 2.0f, 5.0f);
    front       = vec3(0.0f, 0.0f, -1.0f);
    up          = vec3(0.0f, 1.0f, 0.0f);
    yaw         = -90.0f;
    pitch       = 0.0f;
    speed       = 15.0f;
    sensitivity = 0.1f;
}

void Camera::processKeyboard(const float deltaTime, bool forward, bool backward, bool right, bool left) {
    float velocity = speed * deltaTime;
    vec3 right_vec = normalize(cross(front, up));

    if (forward) position += front * velocity;
    if (backward) position -= front * velocity;
    if (left) position += right_vec * velocity;
    if (right) position -= right_vec * velocity;
}

void Camera::processMouse(float offsetX, float offsetY) {
    yaw += offsetX * sensitivity;
    pitch += offsetY * sensitivity;

    // Ограничиваем pitch чтобы не перевернуться
    if (pitch >  89.0f) pitch =  89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Пересчитываем направление взгляда
    vec3 direction;
    direction.x = cos(radians(yaw)) * cos(radians(pitch));
    direction.y = sin(radians(pitch));
    direction.z = sin(radians(yaw)) * cos(radians(pitch));
    front = normalize(direction);

}

mat4 Camera::getView() {
    return lookAt(position, position + front, up);
}
