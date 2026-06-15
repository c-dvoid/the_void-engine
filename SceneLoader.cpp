
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
        else if (type == "model") {
            std::string modelPath;
            float x, y, z, scale;
            ss >> modelPath >> x >> y >> z >> scale;

            SceneObject obj;
            obj.mesh = new Mesh(modelPath);

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