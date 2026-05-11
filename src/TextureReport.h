#pragma once

#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <vector>

/*
 * TextureReport — parser de relatorio_texturas_tienda.txt
 *
 * Estructura del relatorio:
 *   OBJETO: REF_XXX
 *   MATERIAL: MatName
 *     - [Base Color]: ImageName
 *     - [Alpha]: ImageName       (ignorado — usamos solo Base Color)
 *     - [Normal]: ImageName      (ignorado — shader no usa normal maps)
 *   ..............................
 *
 * Resultado: objName → { matName → imageNameBaseColor }
 * Las claves se almacenan en minúsculas para comparación case-insensitive.
 *
 * Normalización de nombres de objeto:
 *   "REF_Rack_Rack_Refri_2" → "ref_rack_refri_2"   (tokens duplicados eliminados)
 *   "REF_Piso_completo_R2R" → "ref_piso_completo_r2r" (lowercase)
 * Esto permite casar los nombres del relatorio con los stems de los archivos .obj.
 */

// matName (lowercase) → imageName (ej. "Image_0.078")
using MatTexMap = std::unordered_map<std::string, std::string>;

// objName normalizado (lowercase, sin tokens duplicados) → MatTexMap
using ObjTexMap = std::unordered_map<std::string, MatTexMap>;

// Convierte a minúsculas.
inline std::string toLower(std::string s)
{
    for (char& c : s) c = (char)std::tolower((unsigned char)c);
    return s;
}

// Elimina tokens consecutivos duplicados separados por '_'.
// "ref_rack_rack_refri_2" → "ref_rack_refri_2"
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

// Normaliza un nombre de objeto para usarlo como clave del mapa.
inline std::string normalizeObjName(const std::string& name)
{
    return deduplicateTokens(toLower(name));
}

// Parsea el relatorio y devuelve el mapa de texturas.
inline ObjTexMap ParseTextureReport(const std::string& reportPath)
{
    ObjTexMap result;
    std::ifstream f(reportPath);
    if (!f.is_open()) {
        std::cerr << "[TextureReport] No se pudo abrir: " << reportPath << "\n";
        return result;
    }

    std::string currentObj, currentMat;
    std::string line;

    while (std::getline(f, line)) {
        // Elimina \r si el archivo tiene line endings Windows.
        if (!line.empty() && line.back() == '\r') line.pop_back();

        if (line.rfind("OBJETO: ", 0) == 0) {
            std::string raw = line.substr(8);
            while (!raw.empty() && std::isspace((unsigned char)raw.back())) raw.pop_back();
            currentObj = normalizeObjName(raw);
            currentMat.clear();
        }
        else if (line.rfind("MATERIAL: ", 0) == 0) {
            std::string raw = line.substr(10);
            while (!raw.empty() && std::isspace((unsigned char)raw.back())) raw.pop_back();
            currentMat = toLower(raw);
        }
        else if (line.find("[Base Color]: ") != std::string::npos) {
            if (currentObj.empty() || currentMat.empty()) continue;

            auto pos = line.find("[Base Color]: ");
            std::string img = line.substr(pos + 14);
            // Trim espacios
            while (!img.empty() && std::isspace((unsigned char)img.back()))  img.pop_back();
            while (!img.empty() && std::isspace((unsigned char)img.front())) img.erase(img.begin());

            // Solo almacena la primera entrada Base Color por (objeto, material).
            auto& matMap = result[currentObj];
            if (matMap.find(currentMat) == matMap.end())
                matMap[currentMat] = img;
        }
    }

    return result;
}

// Busca la imagen Base Color para (objFileStem, materialName).
// objFileStem se normaliza antes de buscar.
// Devuelve "" si no hay entrada en el relatorio.
inline std::string LookupTexture(const ObjTexMap&   report,
                                 const std::string& objFileStem,
                                 const std::string& materialName)
{
    std::string objKey = normalizeObjName(objFileStem);
    std::string matKey = toLower(materialName);

    auto objIt = report.find(objKey);
    if (objIt == report.end()) return "";

    auto matIt = objIt->second.find(matKey);
    if (matIt == objIt->second.end()) return "";

    return matIt->second;
}
