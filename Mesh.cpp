
#include "Mesh.h"

#include <stb_image.h>

#include <iostream>

Mesh::Mesh(const std::string& path) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        // aiProcess_FlipUVs |
        aiProcess_JoinIdenticalVertices |
        aiProcess_GenNormals
        // aiProcess_OptimizeMeshes |
        // aiProcess_OptimizeGraph
    );

    if (!scene || !scene->mRootNode) {
        std::cout << "Failed to load up a model: " << path << std::endl;
        return;
    }

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    processNode(scene->mRootNode, scene, vertices, indices);
    setup(vertices, indices);
}

unsigned int Mesh::loadTexture(const aiScene* scene, int textureIndex) {
    const aiTexture* tex = scene->mTextures[textureIndex];

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, channels;
    unsigned char* data = nullptr;

    if (tex->mHeight == 0) {
        data = stbi_load_from_memory(
            (unsigned char*)tex->pcData,
            tex->mWidth,
            &width, &height, &channels, 0
        );
    } else {
        width = tex->mWidth;
        height = tex->mHeight;
        data = (unsigned char*)tex->pcData;
        channels = 4;
    }

    if (data) {
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        if (tex->mHeight == 0) stbi_image_free(data);
    } else {
        std::cout << "Failed to load embedded texture: " << std::endl;
    }

    return textureID;
}

void Mesh::processNode(aiNode* node, const aiScene* scene,
                       std::vector<Vertex>& vertices,
                       std::vector<unsigned int>& indices) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene, vertices, indices);
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, vertices, indices);
    }
}

void Mesh::processMesh(aiMesh* mesh, const aiScene* scene,
                       std::vector<Vertex>& vertices,
                       std::vector<unsigned int>& indices) {
    unsigned int baseIndex = vertices.size();

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex v;
        v.position = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        };

        if (mesh->HasNormals())
            v.normal = {mesh->mNormals[i].x,
                        mesh->mNormals[i].y,
                        mesh->mNormals[i].z};
        else
            v.normal = {0.0f, 1.0f, 0.0f};

        if (mesh->mTextureCoords[0])
            v.texCoord = {mesh->mTextureCoords[0][i].x,
                          mesh->mTextureCoords[0][i].y};
        else
            v.texCoord = {0.0f, 0.0f};

        vertices.push_back(v);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(baseIndex + face.mIndices[j]);
    }

    // Загружаем текстуру меша
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiString texPath;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
            std::string pathStr = texPath.C_Str();
            // Embedded текстура — путь начинается с *
            if (pathStr[0] == '*') {
                int texIndex = std::stoi(pathStr.substr(1));
                textureIDs.push_back(loadTexture(scene, texIndex));
            }
        }
    }
}

void Mesh::setup(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices) {
    indexCount = indices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Позиция — атрибут 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Нормаль — атрибут 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // UV — атрибут 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

}

void Mesh::draw(Shader& shader) {
    // Привязываем текстуру модели если есть
    if (!textureIDs.empty()) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureIDs[0]);
        shader.setInt("ourTexture", 0);
    } else {
        std::cout << "No textures loaded!" << std::endl;
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    for (unsigned int id : textureIDs)
        glDeleteTextures(1, &id);
}
