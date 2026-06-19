
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <functional>

#include "Mesh.h"
#include "Shader.h"

struct SceneObject {
    Mesh* mesh;
    glm::mat4 transform;
    glm::vec3 color;
    bool hasTexture;
};

struct Trigger {
    glm::vec3 position;
    float radius;
    bool activated;
    std::string action;
    std::string parameter;
};

struct SceneLight {
    glm::vec3 position;
    glm::vec3 color;
};

class Scene {
public:
    std::vector<SceneObject> objects;
    std::vector<Trigger>     triggers;
    SceneLight               light;
    glm::vec3                spawnPoint;
    std::string              nextLevel;
    float                    ambientStrength;

    Scene();
    ~Scene();

    void update(glm::vec3 playerPos, std::function<void(std::string, std::string)> onTrigger);
    void draw(Shader& shader);
};