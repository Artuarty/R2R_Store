#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include <GL/glew.h>
#include "stb_image.h"

/*
 * TextureManager — caché de texturas en GPU.
 *
 * Clave de caché: nombre de imagen tal como aparece en el relatorio
 * (ej. "Image_0.078", "wanted_me.png").  La primera solicitud carga
 * la textura; las siguientes devuelven el ID ya existente en GPU.
 *
 * Resolución de archivo:
 *   - Si el nombre ya termina en .png/.jpg/.jpeg → se usa tal cual.
 *   - Si no → se prueba <nombre>.png y luego <nombre>.jpg.
 *   - Si no se encuentra → textura blanca 1×1 como fallback.
 *
 * stbi_set_flip_vertically_on_load(true) corrige la inversión UV entre
 * Blender (V=0 arriba en PNG) y OpenGL (V=0 abajo).
 */
class TextureManager
{
public:
    explicit TextureManager(const std::string& texturesDir)
        : dir_(texturesDir) {}

    ~TextureManager()
    {
        for (auto& [name, id] : cache_)
            glDeleteTextures(1, &id);
    }

    // Sin copia (maneja recursos GPU)
    TextureManager(const TextureManager&)            = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    // Devuelve el ID de textura GPU para 'imageName'.
    // La textura se carga en la primera llamada y se reutiliza después.
    GLuint Get(const std::string& imageName)
    {
        if (imageName.empty()) return whiteFallback();

        auto it = cache_.find(imageName);
        if (it != cache_.end()) return it->second;

        GLuint id = loadFromDisk(resolve(imageName), imageName);
        cache_[imageName] = id;
        return id;
    }

private:
    std::string                             dir_;
    std::unordered_map<std::string, GLuint> cache_;
    GLuint                                  whiteId_ = 0;

    // Resuelve el nombre de imagen a un path completo en disco.
    std::string resolve(const std::string& name) const
    {
        namespace fs = std::filesystem;

        auto hasExt = [&](const char* ext) {
            size_t el = strlen(ext);
            return name.size() > el &&
                   name.compare(name.size() - el, el, ext) == 0;
        };

        // Si ya tiene extensión conocida, intenta directamente.
        if (hasExt(".png") || hasExt(".jpg") || hasExt(".jpeg")) {
            std::string p = dir_ + '/' + name;
            if (fs::exists(p)) return p;
            // Podría ser un nombre Blender duplicado (.png.001) — intenta sin el sufijo.
        }

        // Prueba añadiendo extensión.
        for (const char* ext : {".png", ".jpg"}) {
            std::string p = dir_ + '/' + name + ext;
            if (fs::exists(p)) return p;
        }

        return ""; // no encontrado → fallback blanco
    }

    GLuint loadFromDisk(const std::string& path, const std::string& name)
    {
        GLuint id;
        glGenTextures(1, &id);

        if (path.empty()) {
            std::cerr << "[TextureManager] No se encontró: " << name << "\n";
            return uploadWhite(id);
        }

        int w, h, ch;
        // Corrige V=0: Blender exporta PNG con fila 0 arriba; OpenGL espera fila 0 abajo.
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 0);

        if (!data) {
            std::cerr << "[TextureManager] stbi_load falló: " << path << "\n";
            return uploadWhite(id);
        }

        GLenum fmt = ch == 1 ? GL_RED : ch == 3 ? GL_RGB : GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        return id;
    }

    GLuint uploadWhite(GLuint id)
    {
        unsigned char px[3] = {255, 255, 255};
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, px);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        return id;
    }

    // Textura blanca compartida para materiales sin imagen en el relatorio.
    GLuint whiteFallback()
    {
        if (whiteId_ == 0) {
            glGenTextures(1, &whiteId_);
            uploadWhite(whiteId_);
            cache_["__white__"] = whiteId_;
        }
        return whiteId_;
    }
};
