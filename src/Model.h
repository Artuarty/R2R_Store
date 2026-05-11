#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// stb_image — STB_IMAGE_IMPLEMENTATION must be defined before the first include
// (done in main.cpp before including this header)
#include "stb_image.h"

#include "Mesh.h"
#include "Shader.h"

GLuint TextureFromFile(const char* path, const std::string& directory)
{
    std::string filename = directory + '/' + std::string(path);

    GLuint texID;
    glGenTextures(1, &texID);

    int w, h, nch;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename.c_str(), &w, &h, &nch, 0);
    if (data) {
        GLenum fmt = (nch == 1) ? GL_RED : (nch == 3) ? GL_RGB : GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        std::cerr << "Texture failed: " << filename << "\n";
        // 1x1 white fallback
        unsigned char white[3] = {255, 255, 255};
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, white);
    }
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texID;
}

class Model
{
public:
    Model() = default;
    explicit Model(const std::string& path) { loadModel(path); }

    void Draw(Shader& shader) {
        for (auto& m : meshes) m.Draw(shader);
    }

private:
    std::vector<Mesh>    meshes;
    std::string          directory;
    std::vector<Texture> textures_loaded;

    void loadModel(const std::string& path)
    {
        Assimp::Importer imp;
        const aiScene* scene = imp.ReadFile(path,
            aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

        if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
            std::cerr << "ASSIMP: " << imp.GetErrorString() << "\n";
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode* node, const aiScene* scene)
    {
        for (GLuint i = 0; i < node->mNumMeshes; i++)
            meshes.push_back(processMesh(scene->mMeshes[node->mMeshes[i]], scene));
        for (GLuint i = 0; i < node->mNumChildren; i++)
            processNode(node->mChildren[i], scene);
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex>  vertices;
        std::vector<GLuint>  indices;
        std::vector<Texture> textures;

        for (GLuint i = 0; i < mesh->mNumVertices; i++) {
            Vertex v;
            v.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
            v.Normal   = mesh->mNormals
                ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
                : glm::vec3(0, 1, 0);
            v.TexCoords = mesh->mTextureCoords[0]
                ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
                : glm::vec2(0);
            vertices.push_back(v);
        }

        for (GLuint i = 0; i < mesh->mNumFaces; i++)
            for (GLuint j = 0; j < mesh->mFaces[i].mNumIndices; j++)
                indices.push_back(mesh->mFaces[i].mIndices[j]);

        // Material colors from MTL
        MaterialData mat;
        if (mesh->mMaterialIndex >= 0) {
            aiMaterial* m = scene->mMaterials[mesh->mMaterialIndex];

            aiColor3D c;
            if (AI_SUCCESS == m->Get(AI_MATKEY_COLOR_AMBIENT,  c)) mat.ambient  = {c.r,c.g,c.b};
            if (AI_SUCCESS == m->Get(AI_MATKEY_COLOR_DIFFUSE,  c)) mat.diffuse  = {c.r,c.g,c.b};
            if (AI_SUCCESS == m->Get(AI_MATKEY_COLOR_SPECULAR, c)) mat.specular = {c.r,c.g,c.b};
            if (AI_SUCCESS == m->Get(AI_MATKEY_COLOR_EMISSIVE, c)) mat.emissive = {c.r,c.g,c.b};

            float val;
            if (AI_SUCCESS == m->Get(AI_MATKEY_SHININESS, val)) mat.shininess = val;
            if (AI_SUCCESS == m->Get(AI_MATKEY_OPACITY,   val)) mat.opacity   = val;

            // Load diffuse textures if present
            auto diffMaps = loadMaterialTextures(m, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffMaps.begin(), diffMaps.end());
        }

        return Mesh(vertices, indices, textures, mat);
    }

    std::vector<Texture> loadMaterialTextures(aiMaterial* mat,
                                               aiTextureType type,
                                               const std::string& typeName)
    {
        std::vector<Texture> textures;
        for (GLuint i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);
            std::string p(str.C_Str());

            bool skip = false;
            for (auto& t : textures_loaded)
                if (t.path == p) { textures.push_back(t); skip = true; break; }

            if (!skip) {
                Texture tex;
                tex.id   = TextureFromFile(str.C_Str(), directory);
                tex.type = typeName;
                tex.path = p;
                textures.push_back(tex);
                textures_loaded.push_back(tex);
            }
        }
        return textures;
    }
};
