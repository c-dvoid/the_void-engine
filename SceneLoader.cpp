
#include "SceneLoader.h"

#include <glm/gtc/matrix_transform.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

Scene* SceneLoader::load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "Failed to load up the scene: " << path << std::endl;
        return nullptr;
    }

    Scene* scene = new Scene();
    std::string line;

    while (std::getline(file, line)) {
        // пропускаем комментарии и пустые строки
        if (line.empty() || line[0] == '#') continue;

        std::istringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "ambient") {
            ss >> scene->ambientStrength;
        }
        else if (type == "light") {
            ss >> scene->light.position.x
               >> scene->light.position.y
               >> scene->light.position.z;
            scene->light.color = glm::vec3(1.0f);
        }
        else if (type == "spawn") {
            ss >> scene->spawnPoint.x
               >> scene->spawnPoint.y
               >> scene->spawnPoint.z;
        }

        else if (type == "npc") {
            NPC npc;
            std::string modelPath;
            float x, y, z, r, g, b;
            ss >> modelPath >> x >> y >> z >> r >> g >> b;

            npc.mesh = new Mesh(modelPath);
            npc.position = glm::vec3(x, y, z);
            npc.color = glm::vec3(r, g, b);
            npc.updateBounds();

            // Читаем waypoints
            std::string waypointLine;
            while (std::getline(file, waypointLine)) {
                if (waypointLine.empty() || waypointLine[0] == '#') continue;
                std::istringstream wss(waypointLine);
                std::string wtype;
                wss >> wtype;

                if (wtype == "end_npc") break;
                if (wtype == "waypoint") {
                    glm::vec3 wp;
                    wss >> wp.x >> wp.y >> wp.z;
                    npc.waypoints.push_back(wp);
                }
                if (wtype == "speed") {
                    wss >> npc.speed;
                }
            }

            scene->npcs.push_back(new NPC(std::move(npc)));
        }

        else if (type == "model") {
            std::string modelPath;
            float x, y, z, scale;
            float r, g, b;
            std::string collisionStr;
            ss >> modelPath >> x >> y >> z >> scale >> r >> g >> b >> collisionStr;

            SceneObject obj;
            obj.mesh = new Mesh(modelPath);
            obj.color = glm::vec3(r, g, b); // Серый по умолчанию
            obj.hasTexture = false;
            obj.hasCollision = (collisionStr ==  "collision"); // Проверка на наличие коллизий

            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, glm::vec3(x, y, z));
            transform = glm::scale(transform, glm::vec3(scale));
            obj.transform = transform;

            // Bounds
            obj.bounds.min = glm::vec3(x, y, z) + obj.mesh->boundsMin * scale;
            obj.bounds.max = glm::vec3(x, y, z) + obj.mesh->boundsMax * scale;

            scene->objects.push_back(obj);
        }

        else if (type == "wall") {
            float x, y, z, w, h, d;
            std::string collisionStr;
            ss >> x >> y >> z >> w >> h >> d >> collisionStr;

            SceneObject obj;
            obj.mesh = nullptr;
            obj.hasTexture = false;
            obj.hasCollision = true;
            obj.color = glm::vec3(0.0f);
            obj.transform = glm::mat4(1.0f);

            obj.bounds.min = glm::vec3(x - w * 0.5f, y - h * 0.5f, z - d * 0.5f);
            obj.bounds.max = glm::vec3(x + w * 0.5f, y + h * 0.5f, z + d * 0.5f);

            scene->objects.push_back(obj);
        }

        else if (type == "trigger") {
            Trigger trigger;
            std::string radiusStr, actionStr;
            ss >> trigger.position.x
               >> trigger.position.y
               >> trigger.position.z
               >> radiusStr
               >> actionStr;

            // читаем остаток строки как параметр
            std::getline(ss, trigger.parameter);
            // убираем пробел в начале
            if (!trigger.parameter.empty() && trigger.parameter[0] == ' ')
                trigger.parameter = trigger.parameter.substr(1);

            trigger.radius = std::stof(radiusStr.substr(radiusStr.find(':') + 1));
            trigger.action = actionStr.substr(actionStr.find(':') + 1);
            trigger.activated = false;

            scene->triggers.push_back(trigger);
        }
    }

    return scene;
}