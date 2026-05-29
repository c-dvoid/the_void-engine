cmake_minimum_required(VERSION 4.2)
project(Test)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(Test
        main.cpp
        Texture.cpp
        Texture.h
        Shader.h
        Shader.cpp)

target_link_libraries(Test
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        glad::glad
        glm::glm
)

target_include_directories(Test PRIVATE
        D:/vcpkg/installed/x64-mingw-dynamic/include
)

# Копируем папки с ресурсами в папку сборки
file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})