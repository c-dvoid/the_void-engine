
#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Collision.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>

struct NPC {
    glm::vec3 position;
    float speed;
    int currentWaypoint;
    std::vector<glm::vec3> waypoints;
    Mesh* mesh;
    glm::vec3 color;
    AABB bounds;

    NPC();
    void update(float deltaTime);
    void updateBounds();
    void draw(Shader& shader);

    NPC(const NPC&) = delete;
    NPC& operator=(const NPC&) = delete;
    NPC(NPC&& other) noexcept;
    NPC& operator=(NPC&&) = default;

    ~NPC();
};
