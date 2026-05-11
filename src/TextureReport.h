#pragma once

#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdio>

/*
 * TextureReport — parser of relatorio_texturas_tienda.txt
 *
 * Each object/material entry now yields a MaterialConfig that encodes:
 *   TEX:<name>  → hasTexture=true,  imageName="<name>" (prefix stripped)
 *   RGB:<r,g,b> → hasTexture=false, r/g/b filled
 *   VAL:<float> → treated as gray fallback (Base Color), or sets roughness/metallic
 *
 * Roughness and Metallic VAL: lines are parsed per material block.
 */

struct MaterialConfig {
    bool        hasTexture = false;
    std::string imageName;                      // only valid when hasTexture=true
    float       r = 0.5f, g = 0.5f, b = 0.5f; // solid color when !hasTexture
    float       roughness = 0.5f;
    float       metallic  = 0.0f;
};

// matName (lowercase) → MaterialConfig
using MatTexMap = std::unordered_map<std::string, MaterialConfig>;

// objName normalizado (lowercase, tokens duplicados eliminados) → MatTexMap
using ObjTexMap = std::unordered_map<std::string, MatTexMap>;

inline std::string toLower(std::string s)
{
    for (char& c : s) c = (char)std::tolower((unsigned char)c);
    return s;
}

// Elimina tokens consecutivos duplicados separados por '_'.
inline std::string deduplicateTokens(const std::string& s)
{
    std::vector<std::string> parts;
    std::istringstream ss(s);
    std::string tok;
    while (std::getline(ss, tok, '_'))
        if (parts.empty() || parts.back() != tok)
            parts.push_back(tok);

    std::string result;
    for (size_t i = 0; i < parts.size(); i++) {
        if (i) result += '_';
        result += parts[i];
    }
    return result;
}

inline std::string normalizeObjName(const std::string& name)
{
    return deduplicateTokens(toLower(name));
}

// Interpreta "TEX:<name>", "RGB:<r>,<g>,<b>", o "VAL:<v>" para Base Color.
static void parseBaseColor(const std::string& val, MaterialConfig& cfg)
{
    if (val.rfind("TEX:", 0) == 0) {
        cfg.hasTexture = true;
        cfg.imageName  = val.substr(4);
    } else if (val.rfind("RGB:", 0) == 0) {
        cfg.hasTexture = false;
        std::sscanf(val.c_str() + 4, "%f,%f,%f", &cfg.r, &cfg.g, &cfg.b);
    }
    // VAL: → mantener gris por defecto
}

// Extrae el float de "VAL:<float>", o devuelve defaultVal.
static float parseValFloat(const std::string& val, float defaultVal)
{
    if (val.rfind("VAL:", 0) == 0) {
        float f = defaultVal;
        std::sscanf(val.c_str() + 4, "%f", &f);
        return f;
    }
    return defaultVal;
}

inline ObjTexMap ParseTextureReport(const std::string& reportPath)
{
    ObjTexMap result;
    std::ifstream f(reportPath);
    if (!f.is_open()) {
        std::cerr << "[TextureReport] No se pudo abrir: " << reportPath << "\n";
        return result;
    }

    auto trim = [](std::string& s) {
        while (!s.empty() && std::isspace((unsigned char)s.back()))  s.pop_back();
        while (!s.empty() && std::isspace((unsigned char)s.front())) s.erase(s.begin());
    };

    std::string currentObj, currentMat, line;

    while (std::getline(f, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();

        if (line.rfind("OBJETO: ", 0) == 0) {
            std::string raw = line.substr(8);
            trim(raw);
            currentObj = normalizeObjName(raw);
            currentMat.clear();
            continue;
        }
        if (line.rfind("MATERIAL: ", 0) == 0) {
            std::string raw = line.substr(10);
            trim(raw);
            currentMat = toLower(raw);
            continue;
        }
        if (currentObj.empty() || currentMat.empty()) continue;

        // Extrae el valor después de "[Key]: " en cualquier posición de la línea.
        auto extractVal = [&](const std::string& key) -> std::string {
            std::string pat = "[" + key + "]: ";
            auto pos = line.find(pat);
            if (pos == std::string::npos) return "";
            std::string v = line.substr(pos + pat.size());
            trim(v);
            return v;
        };

        std::string v;
        if (!(v = extractVal("Base Color")).empty()) {
            parseBaseColor(v, result[currentObj][currentMat]);
        } else if (!(v = extractVal("Roughness")).empty()) {
            result[currentObj][currentMat].roughness = parseValFloat(v, 0.5f);
        } else if (!(v = extractVal("Metallic")).empty()) {
            result[currentObj][currentMat].metallic = parseValFloat(v, 0.0f);
        }
    }

    return result;
}

// Devuelve puntero a MaterialConfig para (objFileStem, materialName), o nullptr si no existe.
inline const MaterialConfig* LookupMaterial(const ObjTexMap&   report,
                                             const std::string& objFileStem,
                                             const std::string& materialName)
{
    std::string objKey = normalizeObjName(objFileStem);
    std::string matKey = toLower(materialName);

    auto objIt = report.find(objKey);
    if (objIt == report.end()) return nullptr;

    auto matIt = objIt->second.find(matKey);
    if (matIt == objIt->second.end()) return nullptr;

    return &matIt->second;
}

// Compatibilidad: devuelve solo el imageName si hay TEX:, cadena vacía en caso contrario.
inline std::string LookupTexture(const ObjTexMap&   report,
                                 const std::string& objFileStem,
                                 const std::string& materialName)
{
    const MaterialConfig* cfg = LookupMaterial(report, objFileStem, materialName);
    if (!cfg || !cfg->hasTexture) return "";
    return cfg->imageName;
}
