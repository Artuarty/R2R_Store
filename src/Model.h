#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <filesystem>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"
#include "TextureManager.h"
#include "TextureReport.h"

/*
 * Model — carga un .obj con Assimp e ignora el .mtl.
 *
 * Las texturas se obtienen de TextureManager usando el relatorio
 * relatorio_texturas_tienda.txt:
 *   1. Se extrae el nombre del objeto del stem del path (ej. "REF_Estante_1").
 *   2. Por cada mesh se pregunta al aiMaterial su nombre (== usemtl en el OBJ).
 *   3. Se busca en el relatorio: (objName, matName) → imageName.
 *   4. TextureManager devuelve el ID GPU (cacheado o cargado en esa llamada).
 *
 * Normalización de nombres (case-insensitive, tokens duplicados eliminados):
 *   "REF_Rack_Rack_Refri_2" → "ref_rack_refri_2"   casa con relatorio "REF_Rack_Refri_2"
 *   "REF_Piso_completo_R2R" → "ref_piso_completo_r2r" casa con "REF_Piso_completo_r2r"
 *
 * Se elimina aiProcess_FlipUVs porque TextureManager ya usa
 * stbi_set_flip_vertically_on_load(true) para corregir la inversión UV.
 */
class Model
{
public:
    Model() = default;

    Model(const std::string& path,
          TextureManager&    texMgr,
          const ObjTexMap&   report)
    {
        loadModel(path, texMgr, report);
    }

    void Draw(Shader& shader) {
        for (auto& m : meshes_) m.Draw(shader);
    }

    // Dibuja solo meshes opacos (opacity==1 y sin tex-alpha) — PASS 1
    void DrawOpaque(Shader& shader) {
        for (auto& m : meshes_)
            if (!m.isTransparent()) m.Draw(shader);
    }

    // Dibuja solo meshes transparentes — PASS 2 (blend debe estar activo)
    void DrawTransparent(Shader& shader) {
        for (auto& m : meshes_)
            if (m.isTransparent()) m.Draw(shader);
    }

private:
    std::vector<Mesh> meshes_;

    void loadModel(const std::string&  path,
                   TextureManager&     texMgr,
                   const ObjTexMap&    report)
    {
        Assimp::Importer imp;
        // aiProcess_FlipUVs omitido: stbi_set_flip_vertically_on_load corrige el UV.
        const aiScene* scene = imp.ReadFile(path,
            aiProcess_Triangulate | aiProcess_GenSmoothNormals);

        if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
            std::cerr << "[Model] Assimp: " << imp.GetErrorString()
                      << " (" << path << ")\n";
            return;
        }

        // Nombre del objeto = stem del archivo, normalizado para lookup en relatorio.
        std::string objKey = normalizeObjName(
            std::filesystem::path(path).stem().string());

        processNode(scene->mRootNode, scene, objKey, texMgr, report);
    }

    void processNode(aiNode*            node,
                     const aiScene*     scene,
                     const std::string& objKey,
                     TextureManager&    texMgr,
                     const ObjTexMap&   report)
    {
        for (unsigned i = 0; i < node->mNumMeshes; i++)
            meshes_.push_back(
                processMesh(scene->mMeshes[node->mMeshes[i]], scene,
                            objKey, texMgr, report));

        for (unsigned i = 0; i < node->mNumChildren; i++)
            processNode(node->mChildren[i], scene, objKey, texMgr, report);
    }

    Mesh processMesh(aiMesh*            mesh,
                     const aiScene*     scene,
                     const std::string& objKey,
                     TextureManager&    texMgr,
                     const ObjTexMap&   report)
    {
        // ── Geometría ──────────────────────────────────────────────────────────
        std::vector<Vertex> vertices;
        vertices.reserve(mesh->mNumVertices);

        for (unsigned i = 0; i < mesh->mNumVertices; i++) {
            Vertex v;
            v.Position = { mesh->mVertices[i].x,
                           mesh->mVertices[i].y,
                           mesh->mVertices[i].z };
            v.Normal   = mesh->mNormals
                ? glm::vec3(mesh->mNormals[i].x,
                            mesh->mNormals[i].y,
                            mesh->mNormals[i].z)
                : glm::vec3(0.0f, 1.0f, 0.0f);
            v.TexCoords = mesh->mTextureCoords[0]
                ? glm::vec2(mesh->mTextureCoords[0][i].x,
                            mesh->mTextureCoords[0][i].y)
                : glm::vec2(0.0f);
            vertices.push_back(v);
        }

        std::vector<GLuint> indices;
        indices.reserve(mesh->mNumFaces * 3u);
        for (unsigned i = 0; i < mesh->mNumFaces; i++)
            for (unsigned j = 0; j < mesh->mFaces[i].mNumIndices; j++)
                indices.push_back(mesh->mFaces[i].mIndices[j]);

        // ── Material desde relatorio ──────────────────────────────────────────
            Texture      diffTex;
            MaterialData mat;
            glm::vec3    solidColor(0.5f); // gris por defecto si no hay entrada en relatorio

            // ====> AGREGAR ESTO PARA FIJAR UN ESTADO OPACO POR DEFECTO <====
            mat.opacity     = 1.0f;  // Por defecto, opaco
            mat.useTexAlpha = false; // No usar alpha de textura
            mat.shininess   = 32.0f; // Valor de shininess genérico
            mat.specular    = glm::vec3(0.1f); // Brillo genérico

        if (mesh->mMaterialIndex < scene->mNumMaterials) {
            aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];

            aiString aiMatName;
            aiMat->Get(AI_MATKEY_NAME, aiMatName);
            std::string matKey = toLower(std::string(aiMatName.C_Str()));

            const MaterialConfig* cfg = LookupMaterial(report, objKey, matKey);
            if (cfg) {
                // 1. Cargar texturas y colores base (mantener igual)
                if (cfg->hasTexture) {
                    diffTex.id = texMgr.Get(cfg->imageName);
                    diffTex.name = cfg->imageName;
                } else {
                    solidColor = glm::vec3(cfg->r, cfg->g, cfg->b);
                }

                // 2. Identificar si es un material de tipo VIDRIO
                bool isGlass = (matKey.find("glass") != std::string::npos ||
                                matKey.find("transparency") != std::string::npos ||
                                matKey.find("vidrio") != std::string::npos);

                // 3. LÓGICA DE TRANSPARENCIA REFORZADA
                mat.useTexAlpha = false; 
                mat.opacity = 1.0f;

                if (isGlass) {
                    // Si es vidrio, forzamos transparencia
                    if (cfg->alphaFromTex && diffTex.id != 0) {
                        mat.useTexAlpha = texMgr.HasAlpha(cfg->imageName);
                    } else {
                        // Si el valor es muy bajo en Blender (0.0), ponemos 0.15 para que se vea el cristal
                        mat.opacity = (cfg->opacity < 0.01f) ? 0.15f : cfg->opacity;
                    }
                } 
                else if (cfg->alphaFromTex && diffTex.id != 0) {
                    // Si NO es vidrio pero tiene textura en Alpha (ej. un logo con transparencia)
                    // Solo activamos useTexAlpha si la imagen REALMENTE tiene canal alfa
                    mat.useTexAlpha = texMgr.HasAlpha(cfg->imageName);
                    
                    // Si la textura NO tiene alpha real, forzamos opacidad total
                    if (!mat.useTexAlpha) mat.opacity = 1.0f;
                } 
                else {
                    // Es un objeto sólido (como tus paredes)
                    mat.opacity = 1.0f;
                }
            }
        }

        return Mesh(std::move(vertices), std::move(indices), diffTex, mat, solidColor);
    }
};
