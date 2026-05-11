#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

// Una textura cargada en GPU (solo guardamos el ID y la fuente para debug).
struct Texture {
    GLuint      id   = 0;
    std::string name; // nombre de imagen del relatorio, ej. "Image_0.078"
};

// Propiedades Blinn-Phong del material.
struct MaterialData {
    glm::vec3 ambient    = glm::vec3(0.15f);
    glm::vec3 diffuse    = glm::vec3(1.00f);
    glm::vec3 specular   = glm::vec3(0.20f);
    glm::vec3 emissive   = glm::vec3(0.00f);
    float     shininess  = 32.0f;
    float     opacity    = 1.0f;
    bool      useTexAlpha = false; // true → shader samplea texture .a para transparencia
};

class Mesh
{
public:
    std::vector<Vertex>  vertices;
    std::vector<GLuint>  indices;
    Texture              diffuseTex;  // Base Color del relatorio (id=0 → sin textura)
    MaterialData         mat;
    glm::vec3            solidColor{0.5f, 0.5f, 0.5f}; // color sólido para materiales RGB:

    // true si este mesh tiene transparencia real (necesita pass 2 con blending)
    bool isTransparent() const {
        return mat.opacity < 1.0f || mat.useTexAlpha;
    }

    Mesh(std::vector<Vertex> v, std::vector<GLuint> i,
         Texture t, MaterialData m,
         glm::vec3 color = glm::vec3(0.5f))
        : vertices(std::move(v)), indices(std::move(i)),
          diffuseTex(t), mat(m), solidColor(color)
    {
        setupMesh();
    }

    void Draw(Shader& shader)
    {
        // Propiedades Blinn-Phong
        shader.setVec3 ("material.ambient",   mat.ambient);
        shader.setVec3 ("material.diffuse",   mat.diffuse);
        shader.setVec3 ("material.specular",  mat.specular);
        shader.setVec3 ("material.emissive",  mat.emissive);
        shader.setFloat("material.shininess", mat.shininess > 0.0f ? mat.shininess : 1.0f);
        shader.setFloat("material.opacity",   mat.opacity);

        // Transparencia: uUseTexAlpha indica al shader que use texture.a
        shader.setBool("uUseTexAlpha", mat.useTexAlpha);

        // Color sólido o textura — siempre se pasa uBaseColor como fallback
        bool hasTex = (diffuseTex.id != 0);
        shader.setBool("material.hasTexture", hasTex);
        shader.setVec3("uBaseColor", solidColor);

        if (hasTex) {
            glActiveTexture(GL_TEXTURE0);
            shader.setInt("texture_diffuse1", 0);
            glBindTexture(GL_TEXTURE_2D, diffuseTex.id);
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
        glBufferData(GL_ARRAY_BUFFER,
                     (GLsizei)(vertices.size() * sizeof(Vertex)),
                     vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     (GLsizei)(indices.size() * sizeof(GLuint)),
                     indices.data(), GL_STATIC_DRAW);

        // location 0 — posición
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void*)offsetof(Vertex, Position));
        // location 1 — normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void*)offsetof(Vertex, Normal));
        // location 2 — UV
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void*)offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
    }
};
