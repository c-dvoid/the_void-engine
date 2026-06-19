#include <glad/glad.h>
#include <SDL2/SDL.h>

#include "Camera.h"
#include "Renderer.h"
#include "Shader.h"
#include "Scene.h"
#include "SceneLoader.h"

#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow(
        "My Game",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL
    );

    int window_Width = SCREEN_WIDTH;
    int window_Height = SCREEN_HEIGHT;

    SDL_GLContext context = SDL_GL_CreateContext(window);
    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    Camera camera;
    Renderer renderer(SCREEN_WIDTH, SCREEN_HEIGHT);
    Shader shader("shaders/landscape_vertex.glsl", "shaders/landscape_fragment.glsl");

    Scene* scene = SceneLoader::load("scenes/level1.map");
    camera.position = scene->spawnPoint;

    bool running = true;
    SDL_Event event;
    float lastFrame = 0.0f;
    float deltaTime = 0.0f;

    SDL_SetRelativeMouseMode(SDL_TRUE);

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN)
                if (event.key.keysym.sym == SDLK_ESCAPE) running = false;
            if (event.type == SDL_WINDOWEVENT)
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    window_Width = event.window.data1;
                    window_Height = event.window.data2;
                    glViewport(0, 0, window_Width, window_Height);
                }
            if (event.type == SDL_MOUSEMOTION)
                camera.processMouse(event.motion.xrel, -event.motion.yrel);
        }

        const Uint8* keys = SDL_GetKeyboardState(NULL);
        camera.processKeyboard(
            deltaTime,
            keys[SDL_SCANCODE_W],
            keys[SDL_SCANCODE_S],
            keys[SDL_SCANCODE_A],
            keys[SDL_SCANCODE_D]
        );

        float currentFrame = SDL_GetTicks() / 1000.0f;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        scene->update(camera.position, [&](std::string action, std::string parameter) {
            if (action == "next_level") {
                delete scene;
                scene = SceneLoader::load("scenes/" + parameter);
                camera.position = scene->spawnPoint;
            }
            if (action == "show_text") {
                std::cout << parameter << std::endl;
            }
        });

        renderer.beginFrame();

        shader.use();
        shader.setVec3("lightPos",
            scene->light.position.x,
            scene->light.position.y,
            scene->light.position.z);
        shader.setVec3("lightColor",
            scene->light.color.x,
            scene->light.color.y,
            scene->light.color.z);
        shader.setVec3("viewPos",
            camera.position.x,
            camera.position.y,
            camera.position.z);
        renderer.applyMatrices(shader, glm::mat4(1.0f), camera);
        scene->draw(shader);

        renderer.endFrame(window);
    }

    delete scene;
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}