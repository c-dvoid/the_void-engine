
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
            std::cout << "Loading NPC..." << std::endl;
            NPC npc;
            std::string modelPath;
            float x, y, z, r, g, b;
            ss >> modelPath >> x >> y >> z >> r >> g >> b;

            npc.mesh = new Mesh(modelPath);
            std::cout << "Mesh created for: " << modelPath << std::endl;

            npc.position = glm::vec3(x, y, z);
            npc.color = glm::vec3(r, g, b);
            npc.updateBounds();
            std::cout << "NPC position set" << std::endl;

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

            std::cout << "Waypoints read: " << npc.waypoints.size() << std::endl;
            scene->npcs.push_back(new NPC(std::move(npc)));
            std::cout << "NPC pushed" << std::endl;
            std::cout << "NPCs loaded: " << scene->npcs.size() << std::endl;
            std::cout << "Objects loaded: " << scene->objects.size() << std::endl;
        }

        else if (type == "model") {
            std::string modelPath;
            float x, y, z, scale;
            float r, g, b;
            ss >> modelPath >> x >> y >> z >> scale >> r >> g >> b;

            SceneObject obj;
            obj.mesh = new Mesh(modelPath);
            obj.color = glm::vec3(r, g, b); // Серый по умолчанию
            obj.hasTexture = false;

            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, glm::vec3(x, y, z));
            transform = glm::scale(transform, glm::vec3(scale));
            obj.transform = transform;

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