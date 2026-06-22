
#pragma once

#include "Shader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <string>


struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

class Mesh {
public:
    glm::vec3 boundsMin;
    glm::vec3 boundsMax;

private:
    unsigned int VAO, VBO, EBO;

    int indexCount;
    std::vector<unsigned int> textureIDs;

    void setup(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

    void processNode(aiNode* node, const aiScene* scene,
                     std::vector<Vertex>& vertices,
                     std::vector<unsigned int>& indices);

    void processMesh(aiMesh* mesh, const aiScene* scene,
                     std::vector<Vertex>& vertices,
                     std::vector<unsigned int>& indices);

    unsigned int loadTexture(const aiScene* scene, int textureIndex);

public:
    Mesh(const std::string& path);
    void draw(Shader& shader);
    ~Mesh();
};