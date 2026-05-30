
#pragma once

#include <glad/glad.h>

#include <iostream>
#include <string>

class Texture {
public:
    unsigned int ID;

    Texture(const std::string& path);
    ~Texture() {
        std::cout << "Texture destroyed, ID: " << ID << std::endl;
        glDeleteTextures(1, &ID);
    }

    void bind();
};