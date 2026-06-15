
#include "Renderer.h"

Renderer::Renderer(int screenWidth, int screenHeight) {

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);

    // Перспектива
    projection = glm::perspective(
        glm::radians(45.0f),
        (float)screenWidth / screenHeight,
        0.1f,
        500.0f
    );
}

void Renderer::beginFrame() {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endFrame(SDL_Window* window) {
    SDL_GL_SwapWindow(window);
}

void Renderer::applyMatrices(Shader& shader, const glm::mat4& model, Camera& camera) {
    shader.setMat4("model", model);
    shader.setMat4("view", camera.getView());
    shader.setMat4("projection", projection);
}