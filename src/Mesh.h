#pragma once

#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "Shader.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    GLuint    id;
    std::string type;
    std::string path;
};

struct MaterialData {
    glm::vec3 ambient  = glm::vec3(0.2f);
    glm::vec3 diffuse  = glm::vec3(0.8f);
    glm::vec3 specular = glm::vec3(0.1f);
    glm::vec3 emissive = glm::vec3(0.0f);
    float     shininess = 32.0f;
    float     opacity   = 1.0f;
};

class Mesh
{
public:
    std::vector<Vertex>  vertices;
    std::vector<GLuint>  indices;
    std::vector<Texture> textures;
    MaterialData         mat;

    Mesh(std::vector<Vertex> v, std::vector<GLuint> i,
         std::vector<Texture> t, MaterialData m)
        : vertices(v), indices(i), textures(t), mat(m)
    {
        setupMesh();
    }

    void Draw(Shader& shader)
    {
        // Material colors from MTL
        shader.setVec3("material.ambient",   mat.ambient);
        shader.setVec3("material.diffuse",   mat.diffuse);
        shader.setVec3("material.specular",  mat.specular);
        shader.setVec3("material.emissive",  mat.emissive);
        shader.setFloat("material.shininess", mat.shininess > 0.0f ? mat.shininess : 1.0f);
        shader.setFloat("material.opacity",  mat.opacity);

        GLuint diffNr = 1;
        bool hasTex = !textures.empty();
        shader.setBool("material.hasTexture", hasTex);

        for (GLuint i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            std::string name = textures[i].type + std::to_string(
                textures[i].type == "texture_diffuse" ? diffNr++ : 1);
            shader.setInt(name.c_str(), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }

private:
    GLuint VAO, VBO, EBO;

    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                     vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                     indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (GLvoid*)offsetof(Vertex, Position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (GLvoid*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (GLvoid*)offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
    }
};
