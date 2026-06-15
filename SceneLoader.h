
#pragma once

#include "Scene.h"

#include <string>

class SceneLoader {
public:
    static Scene* load(const std::string& path);
};