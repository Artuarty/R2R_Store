# Manual Técnico — R2R Store
## Documentación del Proyecto de Laboratorio de Computación Grafica e Interacción Humano-Computadora

**Asignatura:** Laboratorio de Computación Grafica e Interacción Humano-Computadora
**Autor:** Arturo Rodriguez Rodriguez  

---

## 1. Objetivos

### 1.1 Objetivo General
Desarrollar un entorno virtual tridimensional interactivo que represente fielmente una tienda de conveniencia llamada "R2R", aplicando los principios fundamentales de gráficas por computadora: modelado geométrico, transformaciones 3D, iluminación física, texturizado PBR y animación procedimental, utilizando OpenGL 3.3 Core Profile como API de renderizado.

### 1.2 Objetivos Específicos

1. **Construcción de escena:** Integrar un mínimo de 7 objetos únicos texturizados representando el mobiliario y equipamiento característico de una tienda de conveniencia.

2. **Sistema de animación:** Implementar 9 animaciones (4 simples + 3 complejas + 2 de iluminación) generadas íntegramente por código, con justificación matemática documentada para las animaciones complejas.

3. **Iluminación Blinn-Phong:** Implementar un modelo de iluminación físicamente plausible con componentes ambiental, difusa y especular, usando 1 luz direccional y 3 luces puntuales (2 de techo animadas + 1 de barra de cafeteras).

4. **Cámara sintética:** Configurar una cámara libre con control FPS (First Person Shooter) que permita explorar el entorno con 6 grados de libertad translacionales y 2 rotacionales.

5. **Portabilidad total:** Garantizar que el ejecutable funcione en cualquier máquina sin modificar rutas, usando detección dinámica del directorio raíz desde `argv[0]`.

6. **Pipeline de materiales:** Desarrollar un sistema que lea un relatorio de texturas generado desde Blender y aplique automáticamente texturas o colores sólidos según corresponda, incluyendo manejo correcto de transparencias.

---

## 2. Alcance del Proyecto

### 2.1 Incluido

| Categoría | Descripción |
|---|---|
| **Escena** | Tienda completa: estructura, piso, paredes, techo, mobiliario (cafeteras, refrigeradores, estantes, mostradores, cajas registradoras, máquina de hielo) |
| **Modelos** | 76 archivos `.obj` exportados desde Blender; total ~152 archivos incluyendo `.mtl` |
| **Texturas** | 195 imágenes (RGB, RGBA, LA) con caché en GPU para evitar carga duplicada (**Nota: No todas se utilizan**) |
| **Animaciones** | 9 animaciones: ANIM_01–ANIM_07 + ANIM_LETRERO + ANIM_LÁMPARAS |
| **Transparencia** | Tres pases de renderizado (opaco + transparente + bolsa hielo) con blending correcto |
| **Interactividad** | Teclado WASD + mouse libre + teclas 1, E, C, H, R, I para objetos específicos |
| **Portabilidad** | Rutas 100% dinámicas; compila y ejecuta en Linux sin modificación |

### 2.2 Fuera de alcance

- Física de colisiones (el usuario puede atravesar objetos)
- Sonido o audio ambiental
- Sombras dinámicas (shadow maps)
- Reflexiones en tiempo real
- Carga de niveles o escenas múltiples
- Interfaz gráfica de usuario (HUD/menús)

---

## 3. Limitantes

### 3.1 Técnicas

| Limitante | Descripción | Impacto |
|---|---|---|
| **OpenGL 3.3 Core** | Sin acceso a características de OpenGL 4.x (compute shaders, DSA, etc.) | Bajo |
| **Sin shadow maps** | Las sombras no se proyectan sobre la geometría | Visual moderado |
| **Transparencia sin orden de profundidad** | Los objetos transparentes no se ordenan back-to-front dinámicamente por cámara | Visual mínimo |
| **Un shader global** | Todos los objetos usan el mismo programa GLSL; materiales especiales (emisivos) se manejan vía uniforms | Técnico bajo |
| **Normales no invertidas para reflejos** | No hay normal mapping; las normales suaves provienen de Assimp (`aiProcess_GenSmoothNormals`) | Visual moderado |
| **Coordenadas Blender → OpenGL** | La conversión `OBJ_Z = -Blender_Y` está hardcodeada; escenas con otro sistema de coordenadas requerirían ajuste | Técnico |

### 3.2 De alcance académico

- El proyecto simula una tienda real a escala pero no incluye inventario interactivo ni transacciones
- Las animaciones son cíclicas o por toggle; no hay árbol de estados complejo
- El relatorio de texturas es generado externamente (Blender) y no se valida su integridad en tiempo de carga

---

## 4. Metodología de Software Aplicada

Se aplicó una metodología **incremental iterativa** adaptada al contexto académico:

### 4.1 Fases de desarrollo

```
Iteración 1 — Infraestructura base (semana 1-2)
  ├── Configuración CMake + dependencias
  ├── Ventana GLFW + contexto OpenGL 3.3
  ├── Clases Shader, Camera (código base del profesor)
  └── Prueba de triángulo básico

Iteración 2 — Carga de escena (semana 3-4)
  ├── Clase Mesh + VAO/VBO/EBO
  ├── Clase Model con Assimp
  ├── Carga de modelos .obj sin texturas
  └── Cámara FPS funcional

Iteración 3 — Iluminación (semana 5-6)
  ├── Shader Blinn-Phong (lighting.vert / lighting.frag)
  ├── Luz direccional exterior
  ├── 2 luces puntuales (lámparas del techo)
  └── Integración de normales desde Assimp

Iteración 4 — Materiales y texturas (semana 7-8)
  ├── TextureReport: parser del relatorio de Blender
  ├── TextureManager: caché de texturas en GPU
  ├── Soporte TEX:/RGB:/VAL: en parser
  ├── Uniform uBaseColor para colores sólidos
  └── Corrección flip UV (stbi_set_flip_vertically_on_load)

Iteración 5 — Animaciones (semana 9-10)
  ├── ANIM_01: puertas refrigerador (sinusoidal)
  ├── ANIM_02: puerta entrada (toggle + lerp)
  ├── ANIM_03: cámara seguridad (paneo oscilante)
  ├── ANIM_04: cajón registradora (toggle + sinusoidal)
  ├── ANIM_05: cafetera (smoothstep por fases)
  ├── ANIM_06: extracción helado (Bézier cúbica)
  ├── ANIM_07: máquina de hielo (smoothstep + gravedad)
  └── ANIM_LETRERO: neón pulsante (batimiento de ondas)

Iteración 6 — Transparencias y refinamiento (semana 11)
  ├── Dos pases de renderizado (opaco / transparente)
  ├── Detección de canal alpha real en texturas
  ├── Corrección formato LA (2 canales → RGBA)
  └── Ajuste de naming relatorio ↔ archivos OBJ
```

### 4.2 Control de versiones

El proyecto usa **Git** con historial de commits descriptivos. Convención de mensajes: `feat(módulo): descripción`, `fix(módulo): descripción`.

---

## 5. Diagrama de Flujo del Software

```
┌─────────────────────────────────────────────────────────────┐
│                        INICIO                               │
└──────────────────────────┬──────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│  Inicialización                                             │
│  ├── Calcular root = canonical(argv[0]).parent_path()       │
│  ├── glfwInit() + crear ventana 1280×720                    │
│  ├── glewInit() + glEnable(GL_DEPTH_TEST)                   │
│  └── Compilar shader (lighting.vert + lighting.frag)        │
└──────────────────────────┬──────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│  Carga de recursos                                          │
│  ├── ParseTextureReport("relatorio_texturas_tienda.txt")    │
│  │     └── Construye ObjTexMap: {objeto → {mat → config}}  │
│  ├── TextureManager(texturesDir)                            │
│  │     └── Caché vacía (carga lazy en primer uso)          │
│  ├── Por cada .obj en statics/animados:                     │
│  │     ├── Assimp::ReadFile(path, aiProcess_Triangulate)    │
│  │     ├── Por cada mesh:                                   │
│  │     │     ├── LookupMaterial(report, objStem, matName)  │
│  │     │     ├── Si TEX: → texMgr.Get(imageName) → GPU ID  │
│  │     │     ├── Si RGB: → solidColor en Mesh               │
│  │     │     └── Si VAL:0 → opacity=0.15 (vidrio)          │
│  │     └── Construir VAO/VBO/EBO en GPU                     │
│  └── Calcular pivotes de animación (blenderToOBJ)           │
└──────────────────────────┬──────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                   BUCLE PRINCIPAL                           │
│                   ¿glfwWindowShouldClose?                   │
└──────┬───────────────────────────────────────────┬──────────┘
       │ NO                                        │ SÍ
       ▼                                           ▼
┌──────────────────┐                        ┌─────────────┐
│  Input           │                        │   FIN       │
│  ├── glfwPoll   │                        │ glfwTerminate│
│  ├── DoMovement │                        └─────────────┘
│  └── Toggles    │
└────────┬─────────┘
         │
         ▼
┌──────────────────────────────────────────────────────────┐
│  UpdateAnims(deltaTime)                                  │
│  ├── ANIM_02: lerp puerta hacia target (120°/s)         │
│  └── ANIM_07: avanzar hieloTimer [0, 5s)                │
└─────────────────────────┬────────────────────────────────┘
                          │
                          ▼
┌──────────────────────────────────────────────────────────┐
│  Pre-computar estado de animaciones                      │
│  ├── refriAngle = 45·(1+sin(t·0.8))                     │
│  ├── rise05  (smoothstep cafetera)                       │
│  ├── tapaAngle06 + paletaPos06 (Bézier helados)         │
│  ├── puertaAng07 (smoothstep máquina hielo)             │
│  └── emissive07 (batimiento ondas neón)                  │
└─────────────────────────┬────────────────────────────────┘
                          │
                          ▼
┌──────────────────────────────────────────────────────────┐
│  Configurar uniforms globales del shader                 │
│  ├── view, projection, viewPos                          │
│  ├── dirLight (sol exterior)                             │
│  ├── pointLights[0..1] (lámparas techo)                 │
│  └── lampIntensity = 0.85 + 0.1·sin(1.3t) + 0.05·sin(3.7t) │
└─────────────────────────┬────────────────────────────────┘
                          │
                          ▼
┌──────────────────────────────────────────────────────────┐
│  PASS 1 — renderScene(opaco)                             │
│  Por cada modelo → model.DrawOpaque(shader)              │
│  └── Por cada Mesh donde !isTransparent():               │
│        ├── setUniforms (material, textura/color)         │
│        └── glDrawElements(GL_TRIANGLES)                  │
└─────────────────────────┬────────────────────────────────┘
                          │
                          ▼
┌──────────────────────────────────────────────────────────┐
│  PASS 2 — renderScene(transparente)                      │
│  glEnable(GL_BLEND) + glDepthMask(GL_FALSE)             │
│  Por cada modelo → model.DrawTransparent(shader)         │
│  └── Por cada Mesh donde isTransparent():                │
│        ├── setUniforms                                   │
│        └── glDrawElements(GL_TRIANGLES)                  │
│  glDepthMask(GL_TRUE) + glDisable(GL_BLEND)             │
└─────────────────────────┬────────────────────────────────┘
                          │
                          ▼
┌──────────────────────────────────────────────────────────┐
│  PASS 3 — Bolsa de hielo (si nt07 > 0.25)               │
│  glEnable(GL_BLEND) + alphaOverride=0.85                │
│  glDisable(GL_BLEND)                                     │
└─────────────────────────┬────────────────────────────────┘
                          │
                          ▼
                  glfwSwapBuffers ──────► (volver al bucle)
```

---

## 6. Diagrama de Gantt

```
SEMANA        │ 1  │ 2  │ 3  │ 4  │ 5  │ 6  │ 7  │ 8  │ 9  │ 10 │ 11 │
──────────────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┤
INFRAESTRUCTURA
CMake + libs  │████│████│    │    │    │    │    │    │    │    │    │
Ventana GLFW  │████│    │    │    │    │    │    │    │    │    │    │
Shader base   │    │████│    │    │    │    │    │    │    │    │    │
Cámara FPS    │    │████│    │    │    │    │    │    │    │    │    │
──────────────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┤
ESCENA
Modelos .obj  │    │    │████│████│    │    │    │    │    │    │    │
Assimp + Mesh │    │    │████│████│    │    │    │    │    │    │    │
Layout escena │    │    │    │████│████│    │    │    │    │    │    │
──────────────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┤
ILUMINACIÓN
Blinn-Phong   │    │    │    │    │████│████│    │    │    │    │    │
Luces animadas│    │    │    │    │    │████│    │    │    │    │    │
──────────────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┤
MATERIALES
TextureReport │    │    │    │    │    │    │████│████│    │    │    │
TextureManager│    │    │    │    │    │    │████│████│    │    │    │
Shader colores│    │    │    │    │    │    │    │████│    │    │    │
──────────────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┤
ANIMACIONES
ANIM_01-04    │    │    │    │    │    │    │    │    │████│    │    │
ANIM_05-07    │    │    │    │    │    │    │    │    │████│████│    │
Neón + lámpar │    │    │    │    │    │    │    │    │    │████│    │
──────────────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┤
TRANSPARENCIA
Dos pases     │    │    │    │    │    │    │    │    │    │    │████│
Fix alpha real │    │    │    │    │    │    │    │    │    │    │████│
──────────────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┤
DOCUMENTACIÓN
Manual usuario│    │    │    │    │    │    │    │    │    │    │████│
Manual técnico│    │    │    │    │    │    │    │    │    │    │████│
──────────────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┘
                                                              ▲
                                                      Entrega Lab: sem.11
```

---

## 7. Documentación del Código

### 7.1 Arquitectura general

El proyecto sigue una **arquitectura de capas** clásica para motores OpenGL educativos:

```
┌──────────────────────────────────────────────────────────┐
│                      main.cpp                            │
│         (bucle de juego, input, animaciones)             │
├──────────┬──────────┬──────────┬────────────────────────┤
│ Camera.h │  Model.h │ Shader.h │   TextureManager.h     │
│          │          │          │   TextureReport.h       │
├──────────┴──────────┴──────────┴────────────────────────┤
│                      Mesh.h                              │
│          (geometría, VAO/VBO/EBO, Draw)                  │
├──────────────────────────────────────────────────────────┤
│              OpenGL 3.3 Core Profile                     │
│         GLFW · GLEW · Assimp · GLM · stb_image          │
└──────────────────────────────────────────────────────────┘
```

---

### 7.2 `Shader.h` — Compilación y uso de programas GLSL

**Responsabilidad:** Carga, compila y enlaza vertex + fragment shaders. Expone métodos `setInt`, `setFloat`, `setBool`, `setVec3`, `setMat4` para enviar uniforms.

**Método clave:**
```cpp
Shader(const std::string& vertPath, const std::string& fragPath)
```
Proceso interno:
1. Lee ambos archivos con `std::ifstream`
2. `glCreateShader` + `glShaderSource` + `glCompileShader` para cada etapa
3. `glCreateProgram` + `glAttachShader` + `glLinkProgram`
4. Imprime errores de compilación/link vía `glGetShaderInfoLog`

**Uso en el proyecto:**
```cpp
Shader shader(shd + "lighting.vert", shd + "lighting.frag");
shader.Use();
shader.setMat4("model", modelMatrix);
shader.setVec3("viewPos", camera.GetPosition());
```

---

### 7.3 `Camera.h` — Cámara FPS sintética

**Responsabilidad:** Mantiene posición, yaw y pitch. Calcula la matriz `view` vía `glm::lookAt`.

**Parámetros iniciales (definidos en `ESCENA.md`):**
```
Posición: (0.0, 1.7, 4.0)  — entrada de la tienda, altura de ojos
Yaw:      -90.0°            — apunta en dirección -Z (hacia el interior)
Pitch:      0.0°            — horizontal
```

**Método `ProcessMouseMovement`:** Aplica sensibilidad y limita pitch a [-89°, +89°] para evitar gimbal lock.

**Método `ProcessKeyboard`:** Avanza/retrocede/desplaza en el plano XZ (sin volar), usando `deltaTime` para velocidad independiente del framerate.

---

### 7.4 `TextureReport.h` — Parser del relatorio de Blender

**Responsabilidad:** Lee `relatorio_texturas_tienda.txt` y construye un mapa jerárquico:

```
ObjTexMap  =  { "ref_rack_refri_1"  →  MatTexMap }
MatTexMap  =  { "new_material"      →  MaterialConfig }
```

**Estructura `MaterialConfig`:**
```cpp
struct MaterialConfig {
    bool        hasTexture;   // true si Base Color es TEX:
    std::string imageName;    // nombre de la imagen (sin prefijo "TEX:")
    float       r, g, b;      // color sólido si Base Color es RGB:
    float       roughness;    // de [Roughness]: VAL:
    float       metallic;     // de [Metallic]:  VAL:
    float       opacity;      // de [Alpha]: VAL: (0.0→0.15 glass, 1.0 opaco)
    bool        alphaFromTex; // de [Alpha]: TEX: (requiere verificación de canal)
};
```

**Prefijos reconocidos:**
| Prefijo | Significado | Ejemplo |
|---|---|---|
| `TEX:name` | Textura desde archivo | `TEX:Image_0` → carga `Image_0.png` |
| `RGB:r,g,b` | Color sólido | `RGB:0.8,0.8,0.8` → gris claro |
| `VAL:x` | Valor numérico | `VAL:0.6` → roughness = 0.6 |

**Normalización de nombres:**  
`normalizeObjName("REF_Rack_Rack_Refri_2")` → `"ref_rack_refri_2"` (lowercase + deduplicación de tokens consecutivos). Esto permite casar `REF_Rack_Rack_Refri_2.obj` con la entrada `REF_Rack_Refri_2` del relatorio.

---

### 7.5 `TextureManager.h` — Caché de texturas en GPU

**Responsabilidad:** Carga imágenes a GPU exactamente una vez; devuelve el mismo `GLuint` en llamadas sucesivas con el mismo nombre.

**Resolución de archivos:** Dado `"Image_0"`, prueba `Image_0.png` y luego `Image_0.jpg`. Si no se encuentra, retorna una textura blanca 1×1.

**Manejo de formatos:**
| Canales stbi | Acción | Resultado |
|---|---|---|
| 1 (L)  | `GL_RED` | Escala de grises |
| 2 (LA) | Recarga forzando 4 canales | `GL_RGBA` correcto |
| 3 (RGB) | `GL_RGB` | Sin transparencia |
| 4 (RGBA) | `GL_RGBA` | Con transparencia |

**Método `HasAlpha(imageName)`:** Retorna `true` si la imagen original tenía 2 o 4 canales. Usado por `Model` para decidir si activar muestreo del canal alpha en el shader.

---

### 7.6 `Mesh.h` — Unidad de geometría renderizable

**Responsabilidad:** Almacena un array de vértices + índices en GPU (VAO/VBO/EBO) y ejecuta el draw call.

**Estructura `Vertex`:**
```cpp
struct Vertex {
    glm::vec3 Position;   // location 0 en shader
    glm::vec3 Normal;     // location 1
    glm::vec2 TexCoords;  // location 2
};
```

**Estructura `MaterialData`:**
```cpp
struct MaterialData {
    glm::vec3 ambient    = glm::vec3(0.15f);
    glm::vec3 diffuse    = glm::vec3(1.00f);
    glm::vec3 specular   = glm::vec3(0.20f);
    glm::vec3 emissive   = glm::vec3(0.00f);
    float     shininess  = 32.0f;   // de roughness: (1-r)²×128
    float     opacity    = 1.0f;    // 1.0=opaco, 0.15=vidrio (VAL:0 en Blender)
    bool      useTexAlpha = false;  // true → shader samplea texture.a para transparencia
};
```

**Método `isTransparent()`:**
```cpp
bool isTransparent() const {
    return mat.opacity < 1.0f || mat.useTexAlpha;
}
```
Determina si el mesh va al PASS 1 (opaco) o PASS 2 (transparente) en el bucle de render. El flag `useTexAlpha` solo se activa cuando el nombre del material contiene `glass`, `transparency` o `transparent` **y** la textura realmente tiene canal alpha (RGBA o LA), evitando que materiales opacos con textura RGBA terminen en el pase transparente.

**Método `Draw(Shader&)`:** Envía todos los uniforms del material antes del `glDrawElements`. Siempre envía `uBaseColor` y `uUseTexAlpha` para que el shader tenga contexto correcto.

---

### 7.7 `Model.h` — Cargador de archivos OBJ

**Responsabilidad:** Usa Assimp para leer un `.obj`, extrae la geometría y consulta el relatorio para asignar material a cada mesh.

**Proceso de carga:**
```
ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals)
  └── processNode (recursivo, recorre árbol de nodos)
        └── processMesh (por cada aiMesh)
              ├── Construir vector<Vertex> desde aiMesh
              ├── LookupMaterial(report, objKey, matKey)
              ├── Si cfg→hasTexture: texMgr.Get(imageName)
              ├── Si !cfg→hasTexture: solidColor = {r,g,b}
              ├── Roughness → shininess = (1-r)²×128
              ├── Metallic  → specular  = 0.04 + m×0.56
              ├── Opacity:  VAL:0→0.15f, TEX:glass→useTexAlpha
              └── return Mesh(vertices, indices, tex, mat, color)
```

**Métodos de renderizado:**
- `Draw(shader)` — dibuja todos los meshes (para modelos sin transparencia)
- `DrawOpaque(shader)` — solo meshes donde `!isTransparent()`
- `DrawTransparent(shader)` — solo meshes donde `isTransparent()`

**Lógica de transparencia en `processMesh`:**
```cpp
bool isGlassMat = (matKey.find("glass")       != std::string::npos ||
                   matKey.find("transparency") != std::string::npos ||
                   matKey.find("transparent")  != std::string::npos);

if (cfg->alphaFromTex && isGlassMat && diffTex.id != 0)
    mat.useTexAlpha = texMgr.HasAlpha(cfg->imageName); // canal alpha real
else if (cfg->opacity < 0.5f)
    mat.opacity = 0.15f;  // VAL:0 en Blender → tinte de vidrio
else
    mat.opacity = cfg->opacity;
```
Triple verificación (relatorio + nombre de material + canal real en GPU) para evitar falsos positivos de transparencia en materiales opacos con texturas RGBA.

**Nota sobre `aiProcess_FlipUVs`:** Omitido deliberadamente. La corrección UV se hace en `TextureManager` con `stbi_set_flip_vertically_on_load(true)`, que es más robusta y no duplica la inversión.

---

### 7.8 `shaders/lighting.vert` — Vertex Shader

**Responsabilidad:** Transforma vértices a clip space y pasa datos interpolados al fragment shader.

```glsl
// Entradas (layout locations coinciden con Vertex struct)
layout(location=0) in vec3 position;   // Vertex.Position
layout(location=1) in vec3 normal;     // Vertex.Normal
layout(location=2) in vec2 texCoords;  // Vertex.TexCoords

// Salidas al fragment shader
out vec3 FragPos;   // posición en world space (para cálculo de luz)
out vec3 Normal;    // normal transformada con matriz inversa traspuesta
out vec2 TexCoords; // UV coords
```

**Normal transform correcta:**
```glsl
Normal = mat3(transpose(inverse(model))) * normal;
```
Necesario para que las normales permanezcan perpendiculares a la superficie bajo transformaciones no uniformes (escalado no uniforme).

---

### 7.9 `shaders/lighting.frag` — Fragment Shader Blinn-Phong

**Responsabilidad:** Calcula el color final de cada fragmento usando el modelo de iluminación Blinn-Phong con soporte para texturas, colores sólidos y transparencia.

**Modelo de iluminación Blinn-Phong:**

Para la luz direccional:
```
albedo   = texture.rgb  (si hasTexture)  ó  uBaseColor  (si no)
ambient  = light.ambient  × mat.ambient  × albedo
diffuse  = light.diffuse  × max(dot(N,L),0) × mat.diffuse  × albedo
specular = light.specular × pow(max(dot(N,H),0), shininess) × mat.specular
```
donde `H = normalize(L + V)` es el half-vector (Blinn vs Phong clásico).

Para luces puntuales, se añade atenuación:
```
att = 1 / (constant + linear×d + quadratic×d²)
```

**Lógica de alpha (transparencia):**
```glsl
if (alphaOverride >= 0.0)          → alpha = alphaOverride   // ANIM_07 bolsa
else if (uUseTexAlpha && hasTexture) → alpha = texture(..).a  // vidrio tex
else                                 → alpha = material.opacity // VAL: alpha
```

**Uniforms de animación:**
- `lampIntensity` — escala las luces puntuales (ANIM_LÁMPARAS)
- `emissiveIntensity` — intensidad del emisivo animado (ANIM_LETRERO, glow café)
- `uEmissiveColor` — color del emisivo configurable por draw-call (`vec3(0.31,0.76,0.97)` azul R2R, `vec3(0.80,0.40,0.05)` marrón café)
- `alphaOverride` — fuerza alpha específico sin cambiar el material (−1 = usar material.opacity)

El shader ahora acumula **3 luces puntuales** (`pointLights[3]`) en lugar de 2. La tercera es la luz cálida sobre la barra de cafeteras.

---

### 7.10 `main.cpp` — Bucle de juego y animaciones

#### Sistema de coordenadas
```
OBJ_X = Blender_X          (igual)
OBJ_Y = Blender_Z          (altura)
OBJ_Z = -Blender_Y         (profundidad, negado)
```
Función de conversión:
```cpp
glm::vec3 blenderToOBJ(float bx, float by, float bz) {
    return glm::vec3(bx, bz, -by);
}
```

#### Pivotes para animación
Las posiciones del `ESCENA.md` son los orígenes de pivote desde Blender. Para rotar alrededor de un pivote en world space se usa el patrón Traslación–Rotación–Traslación inversa:

```
M_anim = T(pivot) × R(angle) × T(-pivot)
```
Implementado como `pivotRotY(pivot, angleDeg)` y `pivotRotX(pivot, angleDeg)`.

#### Animaciones implementadas

**ANIM_01 — Puertas refrigerador** *(simple, automática)*
```
angle = 45° × (1 + sin(t × 0.8))   →   rango [0°, 90°]
```

**ANIM_02 — Puerta de entrada** *(simple, toggle E)*  
Lerp suave hacia ángulo target a 120°/s usando `deltaTime`:
```cpp
puertaAngle = glm::clamp(puertaAngle + spd * dt, 0, target);
```

**ANIM_03 — Cámara de seguridad** *(simple, automática)*
```
angle = 60° × sin(t × 0.5)   →   paneo oscilante ±60°
```

**ANIM_04 — Cajón registradora** *(simple, toggle R)*
```
offset = 0.15 × (1 + sin(t × 1.2)) × 0.5   →   [0, 0.15 m]
```

**ANIM_05 — Cafetera sirviendo café** *(compleja, 3 fases)*

Usa smoothstep cúbico `f(t) = t²(3−2t)` para evitar discontinuidades de velocidad:
```
Fase 1 (t: 0.0→0.3): espera (chorro aparece)
Fase 2 (t: 0.3→0.8): rise = 0.06 × smoothstep((t−0.3)/0.5)
Fase 3 (t: 0.8→1.0): rise = 0.06 (nivel lleno)
```

**ANIM_06 — Extracción de helado** *(compleja, Bézier cúbica)*

La paleta describe una curva Bézier cúbica:
```
B(t) = (1−t)³P₀ + 3(1−t)²tP₁ + 3(1−t)t²P₂ + t³P₃
```
Con `smoothstep` aplicado al parámetro `t` de cada fase para arranque/frenado suave.

**ANIM_07 — Máquina de hielo** *(compleja, 4 fases, ciclo 5s)*

```
Fase 1 (t: 0.00→0.25): puerta abre   angle = −90° × smoothstep(t/0.25)
Fase 2 (t: 0.25→0.70): bolsa sale    lerp(init, final, smoothstep(tL))
                                       + gravedad: Δy = ½ × g × tL²  (g reducido)
Fase 3 (t: 0.70→0.85): reposo
Fase 4 (t: 0.85→1.00): puerta cierra angle = −90° × (1 − smoothstep(...))
```

**ANIM_LETRERO — Neón pulsante** *(iluminación animada)*

Batimiento de dos ondas senoidales de distinta frecuencia:
```
E(t) = 0.5(1 + sin(2π×0.8×t)) + 0.3(1 + sin(2π×2.1×t))
resultado: clamp(E, 0, 1) × vec3(0.31, 0.76, 0.97)   — azul eléctrico R2R
```

**ANIM_LÁMPARAS — Parpadeo suave** *(iluminación animada)*

Superposición de dos frecuencias para patrón no periódico:
```
I(t) = 0.85 + 0.10×sin(1.3×t) + 0.05×sin(3.7×t)
```

---

#### Sistema de renderizado en 3 pases

```
┌─────────────────────────────────────────────────────────────────┐
│ PASS 1 — OPACOS                                                 │
│   glDepthMask(ON)  GL_BLEND(OFF)                                │
│   Dibuja: toda la escena con DrawOpaque()                       │
│   Resultado: escena sólida completa en framebuffer + Z-buffer   │
├─────────────────────────────────────────────────────────────────┤
│ PASS 2 — TRANSPARENTES                                          │
│   glDepthMask(OFF)  GL_BLEND(ON)  GL_SRC_ALPHA/ONE_MINUS       │
│   Dibuja: toda la escena con DrawTransparent()                  │
│   Resultado: vidrios blended sobre escena opaca                 │
│   depthMask OFF evita que el vidrio bloquee objetos detrás      │
├─────────────────────────────────────────────────────────────────┤
│ PASS 3 — BOLSA DE HIELO (caso especial)                         │
│   GL_BLEND(ON) + alphaOverride=0.85                             │
│   Dibuja: mdlHieloBolsa con opacidad forzada al 85%             │
└─────────────────────────────────────────────────────────────────┘
```

**Por qué depthMask OFF en PASS 2:**  
Los objetos opacos en PASS 1 escriben sus profundidades en el Z-buffer. En PASS 2, el vidrio (más cercano) pasa el test de profundidad y se mezcla con los opacos ya renderizados. Si el vidrio escribiera en el Z-buffer, objetos opacos más alejados pero visibles a través del vidrio serían incorrectamente ocluidos.

---

## 8. Técnicas de Animación

### 8.1 Paradigma utilizado — Animación Procedimental

El proyecto **no usa keyframes** almacenados (es decir, no hay listas de poses guardadas en disco que se interpolen). En su lugar se emplea **animación procedimental**: cada valor de transformación (ángulo, posición, escala) se calcula en tiempo real como una función matemática del tiempo `t = glfwGetTime()`.

```
Keyframe tradicional:     pose[n-1] —LERP→ pose[n]   (interpola entre datos grabados)
Animación procedimental:  f(t) = expresión matemática  (calcula en cada frame)
```

**Justificación de la elección:**
- Los modelos `.obj` exportados desde Blender solo contienen geometría estática (una sola pose).
- La animación procedimental permite velocidad, rango y frecuencia ajustables con una sola constante en el código.
- No requiere archivos de animación adicionales, lo que simplifica la portabilidad del proyecto.

Para las animaciones controladas por el usuario (**ANIM_02**, **ANIM_04**, **ANIM_07**) se combina animación procedimental con una **máquina de estados finitos** de dos estados (abierto/cerrado, activo/inactivo, pausado/en-curso).

---

### 8.2 Máquinas de estado — Animaciones por Toggle

Tres animaciones responden a la entrada del usuario mediante una máquina de dos estados:

```
Estado A ──[tecla presionada]──► Estado B
Estado B ──[tecla presionada]──► Estado A
```

La transición se detecta con **detección de flanco de subida** para evitar que mantener la tecla pulsada cause múltiples toggles:

```cpp
// main.cpp — detección de flanco (edge-triggered)
auto toggleOn = [&](int key, bool& prev) -> bool {
    bool cur  = keys[key];
    bool trig = cur && !prev;   // true SOLO en el primer frame que entra la tecla
    prev = cur;
    return trig;
};
if (toggleOn(GLFW_KEY_1, refriPrev))   refriAbierta  = !refriAbierta;
if (toggleOn(GLFW_KEY_E, puertaPrev))  puertaAbierta = !puertaAbierta;
if (toggleOn(GLFW_KEY_R, cajonPrev))   cajonActivo   = !cajonActivo;
if (toggleOn(GLFW_KEY_C, cafetPrev))   cafetActiva   = !cafetActiva;
if (toggleOn(GLFW_KEY_H, heladoPrev))  heladoActivo  = !heladoActivo;
if (toggleOn(GLFW_KEY_I, hieloPrev))   hieloActivo   = !hieloActivo;
```

Una vez que el estado cambia, la animación procedimental toma el nuevo estado como entrada y calcula la posición frame a frame sin guardar poses intermedias.

---

### 8.3 Animaciones Simples — Por qué son simples

Una animación es **simple** cuando:
- Requiere una sola función matemática continua
- Afecta un único tipo de transformación (solo rotación o solo traslación)
- No tiene fases ni sincronización entre objetos
- No requiere justificación matemática más allá de la función básica

#### ANIM_01 — Puertas de refrigerador *(toggle tecla 1, rotación)*

```cpp
// main.cpp — UpdateAnims()
float target = refriAbierta ? 90.0f : 0.0f;
const float spd = 60.0f;  // grados/segundo
if (refriAngle < target) refriAngle = glm::min(refriAngle + spd * dt, target);
else                     refriAngle = glm::max(refriAngle - spd * dt, target);
```

Máquina de dos estados (cerradas/abiertas) con transición a velocidad constante de 60°/s, idéntica en concepto a ANIM_02. Puertas 1–4 (`i < 4`) rotan en ángulo negativo (abren en sentido −Y) y puertas 5–8 en positivo (+Y) para que se abran en sus bisagras correctas:

```cpp
for (int i = 0; i < 8; i++) {
    float ang = (i < 4) ? -refriAngle : refriAngle;
    draw(trans, *mdlRefri[i], pivotRotY(refriPivot(i), ang));
}
```

#### ANIM_02 — Puerta de entrada *(toggle E, rotación)*

```cpp
// main.cpp — UpdateAnims()
float target = puertaAbierta ? 90.0f : 0.0f;
const float spd = 120.0f;   // grados por segundo
puertaAngle = glm::clamp(puertaAngle + spd * dt, 0.0f, target);
```

Máquina de dos estados + avance lineal a velocidad constante (`spd × dt`). El uso de `deltaTime` garantiza la misma velocidad visual independientemente del framerate. La puerta derecha rota en sentido negativo y la izquierda en positivo para que ambas se abran hacia fuera:

```cpp
draw(trans, mdlPuertaDer, pivotRotY(pvDer, -puertaAngle));
draw(trans, mdlPuertaIzq, pivotRotY(pvIzq,  puertaAngle));
```

#### ANIM_03 — Cámara de seguridad *(automática, rotación)*

```cpp
// main.cpp línea 380
float camaraAngle = 60.0f * sinf(t * 0.5f);
// Rango: [-60°, +60°]  Periodo: 2π/0.5 ≈ 12.6 s
```

Función seno que mapea directamente a un ángulo de paneo. Sin fases.

#### ANIM_04 — Cajón de registradora *(toggle R, traslación)*

```cpp
// main.cpp línea 383
float offset04 = cajonActivo ? 0.15f * (1.0f + sinf(t * 1.2f)) * 0.5f : 0.0f;
// Rango cuando activo: [0, 0.15 m]
```

Cuando el cajón está activo, la posición oscila suavemente entre 0 y 15 cm usando seno. La fórmula `(1+sin)/2` mapea el rango [-1,1] del seno al rango [0,1], que luego se escala a metros. Cuando el cajón está inactivo, `offset04 = 0` de forma instantánea (la transición no usa lerp).

---

### 8.4 Animaciones Complejas — Por qué son complejas

Una animación es **compleja** cuando:
- Está dividida en **múltiples fases secuenciales** que deben sincronizarse
- Usa **interpolación no lineal** (smoothstep, Bézier) para simular movimiento físico plausible
- Coordina **más de un objeto** en tiempo
- O incorpora **física simplificada** (gravedad)

#### ANIM_05 — Cafetera sirviendo café *(3 fases, smoothstep + emisivo)*

**Por qué es compleja:** Dos efectos deben sincronizarse en tiempo (posición en Y de la taza + intensidad del emisivo marrón) a través de un tiempo paramétrico normalizado `ct ∈ [0,1]` dentro de un ciclo de 5 segundos. La animación solo corre cuando está activa (tecla `C`); el timer se reinicia a 0 al desactivarla.

```cpp
// main.cpp — UpdateAnims() / ANIM_05
if (cafetActiva) {
    cafetTimer += dt;
    if (cafetTimer >= 5.0f) cafetTimer -= 5.0f;
} else {
    cafetTimer = 0.0f;   // reset al desactivar
}

// Pre-cómputo en el bucle de render
float ct = fmodf(cafetTimer, 5.0f) / 5.0f;
float rise05 = 0.0f, tazaGlow = 0.0f;
if (ct < 0.3f) {
    tazaGlow = smoothstep(ct / 0.3f) * 0.4f;              // glow aparece
} else if (ct <= 0.8f) {
    rise05   = 0.12f * smoothstep((ct - 0.3f) / 0.5f);   // taza sube 12 cm
    tazaGlow = 0.5f;                                        // glow máximo
} else {
    float fade = smoothstep((ct - 0.8f) / 0.2f);
    rise05   = 0.12f;                                       // taza en nivel lleno
    tazaGlow = (1.0f - fade) * 0.4f;                       // glow desvanece
}
```

El emisivo se envía con `uEmissiveColor = vec3(0.80, 0.40, 0.05)` (marrón cálido), separado del azul R2R del letrero.

**Justificación del smoothstep cúbico:**
```
f(t) = t²(3 − 2t),    t ∈ [0,1]
f'(t) = 6t(1−t)        → derivada = 0 en t=0 y t=1
```
Sin smoothstep (lerp lineal), el nivel de café arrancaría y frenaría de golpe. Con smoothstep la velocidad de subida es 0 al inicio (el café aún no sale) y 0 al final (el vaso se está llenando y frena al límite). La amplitud pasó de 0.06 m a **0.12 m** para que el movimiento sea visualmente más legible.

**Normalización de fase:**  
Cada fase tiene su propio intervalo `[t_inicio, t_fin]`. Para aplicar smoothstep, `t` se remapea al rango `[0,1]` de esa fase:
```
t_local = (ct − t_inicio) / (t_fin − t_inicio)
valor   = amplitud × smoothstep(t_local)
```
Esto garantiza que `f(0)=0` y `f(1)=amplitud` exactamente, sin acumulación de error numérico entre fases.

#### ANIM_06 — Extracción de helado *(3 fases, curva Bézier cúbica, toggle tecla H)*

**Por qué es compleja:** La paleta describe una trayectoria en el espacio 3D que no es una línea recta (debe salir del contenedor siguiendo un arco natural, como si una mano la sostuviera). Adicionalmente, la tapa debe abrirse primero y cerrarse al final, coordinando dos objetos. La animación solo corre cuando está activa (tecla `H`); el timer se reinicia al desactivarla.

**Curva Bézier cúbica** para la trayectoria de la paleta:
```
B(t) = (1−t)³P₀ + 3(1−t)²tP₁ + 3(1−t)t²P₂ + t³P₃
```
Los 4 puntos de control están definidos en world space:
```cpp
// main.cpp líneas 278-281
glm::vec3 bezP0 = pvPaleta1;                              // P0: dentro del contenedor
glm::vec3 bezP1 = pvPaleta1 + glm::vec3(0.0f,  0.25f,  0.0f);  // P1: sube verticalmente
glm::vec3 bezP2 = pvPaleta1 + glm::vec3(0.15f, 0.45f,  0.2f);  // P2: gira hacia afuera
glm::vec3 bezP3 = pvPaleta1 + glm::vec3(0.15f, 0.45f,  0.5f);  // P3: fuera del contenedor
```

**Por qué Bézier y no lerp lineal:** Un lerp lineal haría que la paleta cruzara la pared del contenedor. La curva Bézier, mediante P₁ y P₂ como "atracciones", genera una trayectoria que primero sube, luego gira hacia el cliente — físicamente coherente con cómo se extrae un helado.

```cpp
// main.cpp — tres fases del ciclo de 8 segundos (solo cuando heladoActivo)
float ht = fmodf(heladoTimer6, 8.0f) / 8.0f;
if (ht < 0.33f) {
    // Fase 1: tapa se abre (rot X negativo)
    tapaAngle06 = -45.0f * smoothstep(ht / 0.33f);
} else if (ht < 0.66f) {
    // Fase 2: paleta sigue la curva Bézier
    float tL    = (ht - 0.33f) / 0.33f;
    tapaAngle06 = -45.0f;   // tapa permanece abierta
    paletaPos06 = bezier3(smoothstep(tL), bezP0, bezP1, bezP2, bezP3);
} else {
    // Fase 3: paleta y tapa regresan
    float tL    = (ht - 0.66f) / 0.34f;
    tapaAngle06 = -45.0f * (1.0f - smoothstep(tL));
    paletaPos06 = bezier3(1.0f - smoothstep(tL), bezP0, bezP1, bezP2, bezP3);
}
```

El smoothstep sobre el parámetro `t` de Bézier añade easing (arranque/frenado suave) a lo largo de la curva, separando la forma de la trayectoria (definida por los puntos de control) de la velocidad de recorrido (definida por la curva de easing).

#### ANIM_07 — Máquina de hielo *(4 fases, smoothstep + gravedad newtoniana)*

**Por qué es compleja:** Cuatro fases secuenciales, dos objetos coordinados (compuerta + bolsa), y una simulación de caída gravitacional que añade una componente física al movimiento. La máquina está en la pared izquierda (X ≈ −3.7); la compuerta **cae hacia el interior de la tienda (+X)** rotando sobre el eje Z.

```cpp
// main.cpp — ANIM_07
float nt07 = hieloActivo ? (hieloTimer / 5.0f) : 0.0f;

// Fase 1 — compuerta cae (rot Z: 0° → -90°, tope hacia +X)
if      (nt07 <= 0.25f)
    puertaAng07 = -90.0f * smoothstep(nt07 / 0.25f);
// Fase 2+3 — compuerta permanece abierta
else if (nt07 <= 0.85f)
    puertaAng07 = -90.0f;
// Fase 4 — compuerta se cierra (-90° → 0°)
else
    puertaAng07 = -90.0f * (1.0f - smoothstep((nt07 - 0.85f) / 0.15f));

// Draw call — pivotRotZ en lugar de pivotRotX
draw(trans, mdlHieloPuerta, pivotRotZ(pvHieloPuerta, puertaAng07));
```

**Cambio de eje respecto a la versión anterior:** La rotación pasó de `pivotRotX` a `pivotRotZ` porque la bisagra de la compuerta es horizontal sobre la pared izquierda; una rotación sobre Z es la que hace caer el tope hacia el interior (+X), coherente con la geometría exportada desde Blender.

**Bolsa de hielo — movimiento con gravedad (sale hacia +X):**
```cpp
// bolsaFinal = bolsaInit + vec3(0.8, -0.10, 0.0)  (0.8 m hacia el interior)
// Fase 2 (t: 0.25 → 0.70)
float tL   = (nt07 - 0.25f) / 0.45f;
float ease = smoothstep(tL);

bPos07.x = lerp(bolsaInit.x, bolsaFinal.x, ease);  // sale 0.8 m hacia +X
bPos07.y = lerp(bolsaInit.y, bolsaFinal.y, ease);  // traslación vertical base
bPos07.z = lerp(bolsaInit.z, bolsaFinal.z, ease);  // sin desplazamiento Z

// Caída gravitacional adicional: Δy = ½·g·t² (cinemática newtoniana)
// g reducido (0.015 × -9.8) para ajustar la escala del mundo virtual
bPos07.y += 0.5f * (-9.8f * 0.015f) * tL * tL;
```

**Justificación de la gravedad:**  
La cinemática newtoniana de caída libre es `Δy = ½·g·t²`. Sin este término, la bolsa se desplazaría a altura constante, lo que no simula el peso del hielo. El factor `0.015` escala `g` a las unidades del mundo virtual (donde 1 unidad ≈ 1 metro en la escena Blender exportada). La bolsa avanza 0.8 m en X (hacia el cliente) mientras cae ligeramente en Y, replicando el movimiento real de un dispensador de hielo.

La bolsa además se renderiza en un **PASS 3** separado (después de opacos y vidrios) con `alphaOverride = 0.85` para garantizar blending correcto sobre todos los objetos ya visibles:

```cpp
// main.cpp — PASS 3
if (nt07 > 0.25f) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    drawModel(shader, mdlHieloBolsa,
              pivotTranslate(bPos07 - bolsaInit),
              0.0f, 0.85f);   // alphaOverride = 0.85
    glDisable(GL_BLEND);
}
```

---

### 8.5 Animaciones de Iluminación

Estas animaciones no mueven geometría: modifican **uniforms del shader** en cada frame para crear efectos de luz animada.

#### ANIM_LETRERO — Neón pulsante *(batimiento de ondas)*

```cpp
// main.cpp líneas 413-417
float emissive07 = glm::clamp(
    0.5f*(1.0f + sinf(2.0f*π*0.8f*t)) +
    0.3f*(1.0f + sinf(2.0f*π*2.1f*t)),
    0.0f, 1.0f);
```

La superposición de dos senoides de frecuencias 0.8 Hz y 2.1 Hz genera un **patrón de batimiento**: la interferencia constructiva/destructiva entre ambas produce pulsos irregulares que imitan el parpadeo de tubos de neón. Una sola senoide daría un pulso perfectamente regular, poco realista.

```
F1(t) = 0.5·(1 + sin(2π·0.8·t))   → base lenta
F2(t) = 0.3·(1 + sin(2π·2.1·t))   → modulación rápida
E(t)  = F1(t) + F2(t)              → latido irregular
```

En el shader GLSL el emisivo se añade al color iluminado final:
```glsl
// lighting.frag
result += emissiveIntensity * vec3(0.31, 0.76, 0.97);  // azul eléctrico R2R
```

#### ANIM_LÁMPARAS — Parpadeo suave *(superposición de frecuencias)*

```cpp
// main.cpp línea 346
float lampI = 0.85f + 0.10f*sinf(1.3f*t) + 0.05f*sinf(3.7f*t);
```

Las frecuencias 1.3 y 3.7 son **inconmensurables** (su cociente 3.7/1.3 ≈ 2.846 es irracional), por lo que la suma nunca se repite exactamente. Esto da una variación de intensidad que se percibe aleatoria aunque es completamente determinista. El valor base de 0.85 mantiene las lámparas siempre encendidas; la variación máxima es ±0.15 (rango aproximado: [0.70, 1.00]).

En el shader, `lampIntensity` escala por igual las componentes ambiente, difusa y especular de ambas luces puntuales:
```glsl
// lighting.frag
vec3 amb = light.ambient  * material.ambient  * albedo * att * lampIntensity;
vec3 dif = light.diffuse  * diff * material.diffuse * albedo * att * lampIntensity;
vec3 spc = light.specular * spec * material.specular * att * lampIntensity;
```

---

### 8.6 Tabla resumen de animaciones

| ID | Nombre | Técnica principal | Tipo | Trigger |
|---|---|---|---|---|
| ANIM_01 | Puertas refrigerador | Máquina 2 estados + lerp lineal 60°/s | Simple | Tecla **1** |
| ANIM_02 | Puerta de entrada | Máquina 2 estados + lerp lineal 120°/s | Simple | Tecla **E** |
| ANIM_03 | Cámara de seguridad | Seno directo (paneo oscilante) | Simple | Automática |
| ANIM_04 | Cajón registradora | Máquina 2 estados + seno | Simple | Tecla **R** |
| ANIM_05 | Cafetera sirviendo | Tiempo paramétrico + smoothstep (3 fases) + emisivo café | Compleja | Tecla **C** |
| ANIM_06 | Extracción de helado | Bézier cúbica + smoothstep + coordinación 2 obj | Compleja | Tecla **H** |
| ANIM_07 | Máquina de hielo | Smoothstep + gravedad newtoniana (4 fases) + pivotRotZ | Compleja | Tecla **I** |
| ANIM_08 | Letrero neón | Batimiento de ondas (emisivo azul R2R en shader) | Iluminación | Automática |
| ANIM_09 | Lámparas parpadeo | Superposición frecuencias inconmensurables | Iluminación | Automática |

---

## 9. Técnica de Iluminación — Blinn-Phong

### 9.1 Fundamento teórico

El proyecto implementa el modelo de iluminación **Blinn-Phong**, una variante del modelo de Phong clásico que reemplaza el vector de reflexión `R` por el **half-vector** `H` para calcular el componente especular. Esto elimina el artefacto de corte especular que aparece en Phong cuando el ángulo cámara-luz supera 90°, y es más eficiente de calcular.

```
Phong:      spec = pow(max(dot(R, V), 0), shininess)
            R = 2·(dot(N,L))·N − L   (cálculo de R costoso)

Blinn-Phong: spec = pow(max(dot(N, H), 0), shininess)
             H = normalize(L + V)    (cálculo de H barato)
```

El modelo descompone la contribución de cada luz en tres componentes:

```
I_total = I_ambiente + I_difusa + I_especular

I_ambiente  = Ka × La × albedo
I_difusa    = Kd × Ld × max(dot(N, L), 0) × albedo
I_especular = Ks × Ls × pow(max(dot(N, H), 0), shininess)
```

Donde:
- `Ka`, `Kd`, `Ks` — coeficientes del material (ambient, diffuse, specular)
- `La`, `Ld`, `Ls` — componentes de la luz
- `N` — normal de superficie normalizada
- `L` — dirección hacia la luz, normalizada
- `V` — dirección hacia la cámara (viewDir)
- `H = normalize(L + V)` — half-vector
- `albedo` — color base del material (de textura o color sólido)
- `shininess` — concentración del brillo especular (derivada de roughness: `(1−r)²×128`)

---

### 9.2 Fuentes de luz en la escena

El proyecto usa **4 fuentes de luz**: una direccional y tres puntuales.

#### Luz direccional — Sol exterior

Simula la luz del sol entrando por las ventanas frontales de la tienda. No tiene posición (es infinitamente lejana), solo dirección.

```cpp
// main.cpp líneas 338-341
shader.setVec3("dirLight.direction", glm::vec3(-0.5f, -1.0f, -0.3f));
shader.setVec3("dirLight.ambient",   glm::vec3(0.05f, 0.07f, 0.10f));  // azul ambiente nocturno
shader.setVec3("dirLight.diffuse",   glm::vec3(1.0f,  0.98f, 0.95f));  // blanco cálido diurno
shader.setVec3("dirLight.specular",  glm::vec3(0.3f,  0.3f,  0.3f));   // especular atenuado
```

**Decisión de diseño:** La dirección `(-0.5, -1.0, -0.3)` apunta desde la esquina superior izquierda hacia el interior de la tienda, siguiendo la posición de las ventanas frontales en el layout de la escena.

```glsl
// lighting.frag — CalcDirLight()
vec3 lightDir = normalize(-light.direction);   // inversión: apunta HACIA la luz
float diff    = max(dot(norm, lightDir), 0.0);
vec3  halfway = normalize(lightDir + viewDir);
float spec    = pow(max(dot(norm, halfway), 0.0), material.shininess);
```

#### Luces puntuales — Lámparas del techo

Dos lámparas en el techo iluminan el interior. A diferencia de la luz direccional, las puntuales tienen posición y su intensidad **decae con la distancia** según una función cuadrática inversa:

```
att(d) = 1 / (Kc + Kl·d + Kq·d²)
```

Los coeficientes usados corresponden a una iluminación interior de rango ~10 metros (valores de la tabla de Ogre3D para luces interiores):

```cpp
// main.cpp líneas 360-369
shader.setFloat("pointLights[0].constant",  1.0f);   // Kc — evita división por cero
shader.setFloat("pointLights[0].linear",    0.07f);  // Kl — atenuación lineal suave
shader.setFloat("pointLights[0].quadratic", 0.017f); // Kq — atenuación cuadrática
```

```glsl
// lighting.frag — CalcPointLight()
float dist = length(light.position - fragPos);
float att  = 1.0 / (light.constant
                  + light.linear    * dist
                  + light.quadratic * dist * dist);
// Escalar por lampIntensity (ANIM_09)
vec3 amb = light.ambient  * material.ambient  * albedo * att * lampIntensity;
vec3 dif = light.diffuse  * diff * material.diffuse * albedo * att * lampIntensity;
vec3 spc = light.specular * spec * material.specular * att * lampIntensity;
```

Las posiciones de las lámparas se calculan desde las coordenadas Blender del `ESCENA.md`:

```cpp
// ESCENA Lampara1: (-0.7389, 0.4617, 3.6875) → OBJ(-0.7389, 3.6875, -0.4617)
glm::vec3 lPos0 = blenderToOBJ(-0.7389f, 0.4617f, 3.6875f);
glm::vec3 lPos1 = blenderToOBJ( 2.9418f, 0.4617f, 3.6875f);
```

#### Luz puntual de cafeteras (tercera luz)

Luz cálida dorada sobre la barra de cafeteras, generada por código (no corresponde a ningún objeto de la escena):

```cpp
// main.cpp — tercera luz puntual
const glm::vec3 cafLightClr(1.0f, 0.72f, 0.30f);   // naranja-dorado
glm::vec3 cafLightPos = blenderToOBJ(-2.2f, 4.0f, 2.4f);
shader.setVec3 ("pointLights[2].ambient",   cafLightClr * 0.20f);
shader.setVec3 ("pointLights[2].diffuse",   cafLightClr * 0.90f);
shader.setVec3 ("pointLights[2].specular",  cafLightClr * 0.40f);
shader.setFloat("pointLights[2].linear",    0.22f);
shader.setFloat("pointLights[2].quadratic", 0.20f);
```

La atenuación más fuerte (`linear=0.22, quadratic=0.20`) limita el radio efectivo de esta luz a ~3 metros, confinándola a la zona de cafeteras sin afectar el resto de la tienda. A diferencia de las lámparas del techo, esta luz **no está animada** (intensidad constante).

---

### 9.3 Pipeline de iluminación en el shader

El fragment shader calcula la contribución de todas las luces y las suma:

```glsl
// lighting.frag — main()
vec3 norm    = normalize(Normal);
vec3 viewDir = normalize(viewPos - FragPos);

// Albedo: de textura o color sólido (según relatorio)
vec3 albedo = material.hasTexture
    ? texture(texture_diffuse1, TexCoords).rgb
    : uBaseColor;

// Suma de contribuciones de todas las fuentes
vec3 result = CalcDirLight(dirLight, norm, viewDir, albedo);
for (int i = 0; i < 3; i++)                          // 2 lámparas + 1 cafetera
    result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, albedo);

// Emisivo del material (logos, objetos brillantes)
result += material.emissive;
result += emissiveIntensity * uEmissiveColor;  // color configurable por draw-call
```

**Normalización de normales:**  
Las normales se renormalizan en el fragment shader (`normalize(Normal)`) porque la interpolación baricéntrica de Gouraud puede producir normales de longitud != 1. Sin esto, los cálculos de `dot(N, L)` darían resultados incorrectos.

**Normal transform en el vertex shader:**  
Las normales no se transforman con la matriz `model` directamente, sino con la inversa traspuesta:

```glsl
// lighting.vert
Normal = mat3(transpose(inverse(model))) * normal;
```

Esto es necesario porque ante transformaciones no uniformes (escalado diferente en cada eje), la normal matemáticamente correcta es perpendicular al plano tangente transformado, que no coincide con `model × normal`. La inversa traspuesta garantiza la perpendicularidad.

---

### 9.4 Materiales y su relación con la iluminación

Los parámetros del material se derivan del relatorio de Blender (campos `[Roughness]` y `[Metallic]`) y se convierten a los parámetros del modelo Blinn-Phong:

```cpp
// Model.h — processMesh()
// Roughness → shininess: materiales más rugosos tienen lóbulo especular más amplio
float rgh = glm::clamp(cfg->roughness, 0.0f, 1.0f);
mat.shininess = glm::max(1.0f, (1.0f - rgh) * (1.0f - rgh) * 128.0f);
// rgh=0.0 → shininess=128  (muy brillante, como metal pulido)
// rgh=0.5 → shininess= 32  (semi-rugoso, como plástico)
// rgh=1.0 → shininess=  1  (mate, como tela o cartón)

// Metallic → intensidad especular: metales reflejan mucho más que no-metales
float met = glm::clamp(cfg->metallic, 0.0f, 1.0f);
mat.specular = glm::vec3(0.04f + met * 0.56f);
// met=0.0 → specular=0.04  (F0 dieléctrico estándar, ~4% reflectancia)
// met=1.0 → specular=0.60  (metal conductor de alta reflectancia)
```

El valor base `0.04` corresponde a la **reflectancia F0 de Fresnel** para materiales dieléctricos, una convención de los modelos PBR (Physically Based Rendering). Aunque el shader usa Blinn-Phong y no PBR completo, este mapeo respeta los valores reales exportados por el Principled BSDF de Blender.

---

### 9.5 Componente ambiental global

El color de fondo (`glClearColor`) y el ambiente de la luz direccional están ajustados para simular la iluminación interior de noche/tarde:

```cpp
glClearColor(0.05f, 0.07f, 0.10f, 1.0f);  // azul muy oscuro (cielo nocturno exterior)
shader.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.07f, 0.10f));
```

Este tono azul oscuro en el ambiente evita que las zonas no iluminadas directamente sean completamente negras, creando la ilusión de luz ambiental dispersa del exterior.

---

## 10. Conclusiones

### 10.1 Técnicas

1. **Pipeline OpenGL moderno:** La adopción de OpenGL Core Profile (sin funciones deprecated) disciplinó el uso correcto de VAOs, shaders GLSL y uniforms explícitos, evitando dependencias del estado global de OpenGL legacy.

2. **Separación de responsabilidades:** La arquitectura en capas (Shader → Mesh → Model → main) demostró ser robusta: cambios en el sistema de transparencia solo requirieron modificar Mesh y Model sin tocar el shader de iluminación base.

3. **Parser de relatorio:** El enfoque de leer un archivo de texto generado automáticamente por Blender eliminó el trabajo manual de asignación de materiales para 76 modelos. El sistema de normalización de nombres (`deduplicateTokens + toLower`) fue crítico para absorber inconsistencias entre nombres de objetos y nombres de archivos.

4. **Transparencia en dos pases:** La solución de renderizar en PASS 1 (opaco) y PASS 2 (transparente con `glDepthMask(GL_FALSE)`) resolvió el problema clásico de transparencia sin necesitar ordenamiento dinámico back-to-front, suficiente para la escena estática de la tienda.

5. **Manejo de formatos de textura:** La detección de imágenes LA (Luminance+Alpha, 2 canales) y su conversión forzada a RGBA antes de cargar a GPU fue una solución crítica que no era evidente inicialmente.

### 10.2 Académicas

1. El proyecto demostró que el código base del profesor (Shader, Camera) puede extenderse limpiamente siguiendo sus mismas convenciones, sin reescribir funcionalidad ya probada.

2. La justificación matemática de las animaciones complejas (smoothstep cúbico, curva de Bézier, simulación de gravedad, batimiento de ondas) transformó efectos visuales en demostraciones matematicas de conceptos de álgebra lineal y análisis numérico aplicados a gráficas.

3. El requisito de portabilidad absoluta (sin rutas hardcodeadas) forzó el buen hábito de calcular todas las rutas desde `argv[0]` con `std::filesystem::canonical`, una práctica estándar en software distribuido.

### 8.3 De proceso

El desarrollo iterativo permitió identificar y corregir problemas de integración progresivamente (mismatch de nombres relatorio/archivos, canal alpha en texturas RGB, formato LA no manejado) sin afectar la funcionalidad ya estable en cada iteración anterior. El uso de Git con mensajes descriptivos mantuvo un historial de cambios que facilita la revisión académica.

---

## 11. Referencias Técnicas

| Recurso | Uso |
|---|---|
| LearnOpenGL.com — Joey de Vries | Base de Shader, Camera, Mesh, Model |
| OpenGL Reference Pages (khronos.org) | API calls: `glTexImage2D`, `glBlendFunc`, `glDepthMask` |
| GLM Documentation | Matemáticas: `lookAt`, `perspective`, `rotate`, `translate` |
| Assimp Documentation | Flags de post-procesado, acceso a aiMesh, aiMaterial |
| stb_image (nothings.org) | Carga de PNG/JPG, conversión de canales, flip vertical |
| Real-Time Rendering, 4th ed. — Akenine-Möller | Modelo Blinn-Phong, atenuación de luces puntuales |
| Blender Manual — Principled BSDF | Interpretación de canales Alpha, Roughness, Metallic |
