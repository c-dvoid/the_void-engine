
#include "Scene.h"

#include <iostream>

Scene::Scene() {
    ambientStrength = 0.6f;
    spawnPoint = glm::vec3(0.0f);
    light.position = glm::vec3(5.0f, 10.0f, 5.0f);
    light.color = glm::vec3(1.0f);
}

Scene::~Scene() {
    for (auto& obj : objects) {
        delete obj.mesh;
    }
}

void Scene::update(glm::vec3 playerPos,
    std::function<void(std::string, std::string)> onTrigger) {
    for (auto& trigger : triggers) {
        if (trigger.activated) continue;

        float dist = glm::distance(playerPos, trigger.position);
        if (dist < trigger.radius) {
            trigger.activated = true;
            onTrigger(trigger.action, trigger.parameter);
        }
    }
}

void Scene::draw(Shader& shader) {
    for (auto& obj : objects) {
        shader.setMat4("model", obj.transform);
        shader.setVec3("baseColor", obj.color.x, obj.color.y, obj.color.z);
        shader.setBool("hasTexture", obj.hasTexture);
        obj.mesh->draw(shader);
    }
}
