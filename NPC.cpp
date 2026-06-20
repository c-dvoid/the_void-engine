
#include "NPC.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

NPC::NPC() {
    position = glm::vec3(0.0f);
    speed = 2.0f;
    currentWaypoint = 0;
    mesh = nullptr;
    color = glm::vec3(0.2f, 0.4f, 0.8f);
    updateBounds();
}

NPC::NPC(NPC&& other) noexcept {
    position = other.position;
    speed = other.speed;
    currentWaypoint = other.currentWaypoint;
    waypoints = std::move(other.waypoints);
    mesh = other.mesh;
    color = other.color;
    bounds = other.bounds;
    other.mesh = nullptr;
}

void NPC::updateBounds() {
    bounds.min = position - glm::vec3(1.0f, 0.0f, 1.0f);
    bounds.max = position + glm::vec3(1.0f, 2.0f, 1.0f);
}

void NPC::update(float deltaTime) {
    if (waypoints.empty()) return;

    glm::vec3 target = waypoints[currentWaypoint];
    glm::vec3 direction = target - position;
    float distance = glm::length(direction);

    if (distance < 0.1f) {
        currentWaypoint = (currentWaypoint + 1) % waypoints.size();
    } else {
        position += glm::normalize(direction) * speed * deltaTime;
    }

    updateBounds(); // Обновляем границы коллизии после движения
}

void NPC::draw(Shader& shader) {
    if (!mesh) return;

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);

    shader.setMat4("model", transform);
    shader.setVec3("baseColor", color.x, color.y, color.z);
    shader.setBool("hasTexture", false);

    mesh->draw(shader);
}

NPC::~NPC() {
    delete mesh;
}
