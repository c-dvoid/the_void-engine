// Объявление вершинного шейдера
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform float screen_aspect;

void main() {
    gl_Position = vec4(aPos.x * screen_aspect, aPos.yz, 1.0);
    TexCoord = aTexCoord;
}
