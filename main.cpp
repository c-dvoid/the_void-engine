
#include <glad/glad.h>
#include <SDL2/SDL.h>

#include "Shader.h"
#include "Texture.h"


const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;


int main(int argc, char* argv[]) {

    // Инициализация SDL
    SDL_Init(SDL_INIT_VIDEO);
    // Говорим SDL что хотим OpenGL 3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Создаём окно
    SDL_Window* window = SDL_CreateWindow(
        "My Game",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL
    );

    int window_Width = 0;
    int window_Height = 0;

    SDL_GetWindowSize(window, &window_Width, &window_Height);


    // Создаём OpenGL контекст + Инициализируем GLAD
    SDL_GLContext context = SDL_GL_CreateContext(window);
    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    unsigned int shader = loadShader("shaders/vertex.glsl", "shaders/fragment.glsl");

    unsigned int texture = loadTexture("assets/textures/texture.jpg");

    // Вершины треугольника
    float vertices[] = {
        // x      y      z     u     v
        0.5f,  0.5f,  0.0f,  1.0f, 1.0f,  // top right
        0.5f, -0.5f,  0.0f,  1.0f, 0.0f,  // bottom right
       -0.5f, -0.5f,  0.0f,  0.0f, 0.0f,  // bottom left
       -0.5f,  0.5f,  0.0f,  0.0f, 1.0f   // top left
   };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    // VAO + VBO + EBO(for rectangles or to save memory, specifically)
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // UV — атрибут 1, 2 числа, шаг 5 floats, смещение 3 floats
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Отвязываем VBO
    glBindVertexArray(0);

    // Главный цикл
    bool running = true;
    SDL_Event event;

    while (running) {
        // Обработка событий
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
        }

        float aspect = (float)window_Height / window_Width;


        // Очищаем экран тёмно-синим цветом
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Используем скомпилированные шейдеры
        glUseProgram(shader);

        // Используем текстуру
        glBindTexture(GL_TEXTURE_2D, texture);

        int uni_loc = glGetUniformLocation(shader, "screen_aspect");
        glUniform1f(uni_loc, aspect);

        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Показываем кадр
        SDL_GL_SwapWindow(window);
    }
    // Очистка

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader);
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}