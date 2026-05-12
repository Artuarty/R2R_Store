#pragma once
// Generadores de geometría procedural para objetos de decoración de la tienda.
// Todas las figuras están centradas en el origen; usa glm::translate para posicionarlas.
// Devuelven Mesh con solidColor como albedo (sin textura), listos para Draw().

#include <vector>
#include <cmath>
#include <initializer_list>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "Mesh.h"

// ── Caja ────────────────────────────────────────────────────────────────────
// Centrada en el origen, semi-extents (hw, hh, hd).
// Normales planas por cara para iluminación Blinn-Phong correcta.
inline Mesh makeBox(float hw, float hh, float hd,
                    glm::vec3 color,
                    float     shininess = 32.0f,
                    glm::vec3 specular  = glm::vec3(0.25f))
{
    struct Face { glm::vec3 n; glm::vec3 c[4]; };
    const Face faces[6] = {
        {{ 0, 0, 1},{{-hw,-hh, hd},{ hw,-hh, hd},{ hw, hh, hd},{-hw, hh, hd}}},
        {{ 0, 0,-1},{{ hw,-hh,-hd},{-hw,-hh,-hd},{-hw, hh,-hd},{ hw, hh,-hd}}},
        {{-1, 0, 0},{{-hw,-hh,-hd},{-hw,-hh, hd},{-hw, hh, hd},{-hw, hh,-hd}}},
        {{ 1, 0, 0},{{ hw,-hh, hd},{ hw,-hh,-hd},{ hw, hh,-hd},{ hw, hh, hd}}},
        {{ 0, 1, 0},{{-hw, hh, hd},{ hw, hh, hd},{ hw, hh,-hd},{-hw, hh,-hd}}},
        {{ 0,-1, 0},{{-hw,-hh,-hd},{ hw,-hh,-hd},{ hw,-hh, hd},{-hw,-hh, hd}}},
    };
    const glm::vec2 uvs[4] = {{0,0},{1,0},{1,1},{0,1}};

    std::vector<Vertex> verts;
    std::vector<GLuint> idx;
    verts.reserve(24);
    idx.reserve(36);

    for (int f = 0; f < 6; ++f) {
        GLuint base = (GLuint)verts.size();
        for (int v = 0; v < 4; ++v) {
            Vertex vt;
            vt.Position  = faces[f].c[v];
            vt.Normal    = faces[f].n;
            vt.TexCoords = uvs[v];
            verts.push_back(vt);
        }
        idx.insert(idx.end(), {base,base+1,base+2, base,base+2,base+3});
    }

    MaterialData mat;
    mat.specular  = specular;
    mat.shininess = shininess;
    return Mesh(std::move(verts), std::move(idx), Texture{}, mat, color);
}

// ── Cilindro ─────────────────────────────────────────────────────────────────
// Eje vertical (Y), centrado en origen. Incluye tapas superior e inferior.
// segs: subdivisiones laterales (12-20 es suficiente para objetos pequeños).
inline Mesh makeCylinder(float radius, float height, int segs,
                         glm::vec3 color,
                         float     shininess = 64.0f,
                         glm::vec3 specular  = glm::vec3(0.35f))
{
    std::vector<Vertex> verts;
    std::vector<GLuint> idx;
    const float hh  = height * 0.5f;
    const float TAU = glm::two_pi<float>();

    // ── caras laterales ──────────────────────────────────────────────────────
    for (int i = 0; i < segs; ++i) {
        float a0 = TAU * i / segs,     a1 = TAU * (i+1) / segs;
        float c0 = cosf(a0), s0 = sinf(a0);
        float c1 = cosf(a1), s1 = sinf(a1);
        GLuint base = (GLuint)verts.size();
        Vertex v[4] = {
            {{radius*c0,-hh,radius*s0},{c0,0,s0},{(float)i/segs,0}},
            {{radius*c1,-hh,radius*s1},{c1,0,s1},{(float)(i+1)/segs,0}},
            {{radius*c1, hh,radius*s1},{c1,0,s1},{(float)(i+1)/segs,1}},
            {{radius*c0, hh,radius*s0},{c0,0,s0},{(float)i/segs,1}},
        };
        for (auto& vt : v) verts.push_back(vt);
        idx.insert(idx.end(), {base,base+1,base+2, base,base+2,base+3});
    }

    // ── tapa superior ────────────────────────────────────────────────────────
    {
        GLuint c = (GLuint)verts.size();
        verts.push_back({{0,hh,0},{0,1,0},{0.5f,0.5f}});
        for (int i = 0; i < segs; ++i) {
            float a0 = TAU*i/segs, a1 = TAU*(i+1)/segs;
            GLuint v0 = (GLuint)verts.size();
            verts.push_back({{radius*cosf(a0),hh,radius*sinf(a0)},{0,1,0},
                             {0.5f+0.5f*cosf(a0),0.5f+0.5f*sinf(a0)}});
            verts.push_back({{radius*cosf(a1),hh,radius*sinf(a1)},{0,1,0},
                             {0.5f+0.5f*cosf(a1),0.5f+0.5f*sinf(a1)}});
            idx.insert(idx.end(), {c, v0, v0+1});
        }
    }

    // ── tapa inferior ────────────────────────────────────────────────────────
    {
        GLuint c = (GLuint)verts.size();
        verts.push_back({{0,-hh,0},{0,-1,0},{0.5f,0.5f}});
        for (int i = 0; i < segs; ++i) {
            float a0 = TAU*i/segs, a1 = TAU*(i+1)/segs;
            GLuint v0 = (GLuint)verts.size();
            verts.push_back({{radius*cosf(a0),-hh,radius*sinf(a0)},{0,-1,0},
                             {0.5f+0.5f*cosf(a0),0.5f+0.5f*sinf(a0)}});
            verts.push_back({{radius*cosf(a1),-hh,radius*sinf(a1)},{0,-1,0},
                             {0.5f+0.5f*cosf(a1),0.5f+0.5f*sinf(a1)}});
            idx.insert(idx.end(), {c, v0+1, v0}); // giro inverso para normal hacia abajo
        }
    }

    MaterialData mat;
    mat.specular  = specular;
    mat.shininess = shininess;
    return Mesh(std::move(verts), std::move(idx), Texture{}, mat, color);
}
