
#pragma once

#include <SDL_stdinc.h>

#include "Camera.h"
#include "Collision.h"

#include <glm/glm.hpp>

class Scene;

class Player {
public:
    Camera camera;
    AABB bounds;
    float height;

    Player();

    void update(float deltaTime, const Uint8* keys, Scene* scene);
    void processMouse(float offsetX, float offsetY);
    void updateBounds();

    glm::vec3 getPosition() const;

};
