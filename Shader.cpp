
#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

static std::string readFile(const char* path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "Failed to open up shader: " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static unsigned int compileShader(const char* source, GLenum type) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, log);
        std::cout << "Shader mistake: " << log << std::endl;
    }
    return shader;
}

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    std::string vertCode = readFile(vertexPath);
    std::string fragCode = readFile(fragmentPath);

    const char* vertSource = vertCode.c_str();
    const char* fragSource = fragCode.c_str();

    unsigned int vert = compileShader(vertSource, GL_VERTEX_SHADER);
    unsigned int frag = compileShader(fragSource, GL_FRAGMENT_SHADER);

    ID = glCreateProgram();
    glAttachShader(ID, vert);
    glAttachShader(ID, frag);
    glLinkProgram(ID);

    int success;
    char log[512];
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, log);
        std::cout << "Link-ovka mistake: " << log << std::endl;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);

}

void Shader::use() {
    glUseProgram(ID);
}

void Shader::setFloat(const std::string& name, float value) {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setInt(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setBool(const std::string& name, bool value) {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}