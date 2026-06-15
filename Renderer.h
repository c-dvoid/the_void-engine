
#pragma once

#include "Camera.h"
#include "Shader.h"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Renderer {
public:
    glm::mat4 projection;

    Renderer(int screenWidth, int screenHeight);

    void beginFrame();
    void endFrame(SDL_Window* window);
    void applyMatrices(Shader& shader, const glm::mat4& model, Camera& camera);
};
