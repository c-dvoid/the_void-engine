
#include "Player.h"
#include "Scene.h"

#include <SDL2/SDL.h>

Player::Player() {
    height = 1.7f;
    camera.position = glm::vec3(0.0f, height, 0.0f);
    updateBounds();
}

void Player::update(float deltaTime, const Uint8* keys, Scene* scene) {
    glm::vec3 oldPosition = camera.position;

    camera.processKeyboard(
        deltaTime,
        keys[SDL_SCANCODE_W],
        keys[SDL_SCANCODE_S],
          keys[SDL_SCANCODE_A],
           keys[SDL_SCANCODE_D]
        );

    camera.position.y = height;
    updateBounds();

    // Проверяем, произошла ли коллизия с NPC
    for (auto& npc : scene->npcs) {
        if (!npc) {
            std::cout << "NPC is null!" << std::endl;
            continue;
        }

        if (intersect(bounds, npc->bounds)) {
            std::cout << "Collision!" << std::endl;
            camera.position = oldPosition;
            camera.position.y = height;
            updateBounds();
            break;
        }
    }

    // Проверяем, произошла ли коллизия с любыми другими объектами
    for (auto& obj : scene->objects) {
        if (!obj.hasCollision) continue;
        std::cout << "Player pos: " << camera.position.x << " " << camera.position.y << " " << camera.position.z << std::endl;
        std::cout << "Object bounds min: " << obj.bounds.min.x << " " << obj.bounds.min.y << " " << obj.bounds.min.z << std::endl;
        std::cout << "Object bounds max: " << obj.bounds.max.x << " " << obj.bounds.max.y << " " << obj.bounds.max.z << std::endl;
        if (intersect(bounds, obj.bounds)) {
            std::cout << "Wall collision!" << std::endl;
            camera.position = oldPosition;
            camera.position.y = height;
            updateBounds();
            break;
        }
    }
}

void Player::processMouse(float offsetX, float offsetY) {
    camera.processMouse(offsetX, offsetY);
}

void Player::updateBounds() {
    bounds.min = camera.position - glm::vec3(0.4f, height, 0.4f);
    bounds.max = camera.position + glm::vec3(0.4f, 0.1f, 0.4f);
}

glm::vec3 Player::getPosition() const {
    return camera.position;
}
