# R2R Store — Tienda de Conveniencia Virtual 3D

Entorno virtual interactivo de una tienda de conveniencia desarrollado con **OpenGL 3.3 Core Profile** y **C++17** como proyecto final de Computación grafica e interacción humano-computadora.

**Asignatura:** Gráficas por Computadora  
**Autor:** Arturo Rodriguez Rodriguez  
**Video explicativo**: [Ver Video](https://drive.google.com/file/d/1Pcd8GKSvpgP_QvtqlIVaU91jFcARP3Xy/view?usp=drive_link)

---

## Vista rápida

- 76 modelos `.obj` exportados desde Blender con materiales y texturas PBR
- Iluminación **Blinn-Phong** con 1 luz direccional y 3 luces puntuales
- **9 animaciones** procedimentales (4 simples + 3 complejas + 2 de iluminación)
- Sistema de transparencia en 3 pases (opacos → vidrio → bolsa de hielo)
- Cámara FPS libre con control WASD + mouse
- Rutas 100 % dinámicas: el ejecutable funciona en cualquier máquina sin modificar nada

---

## Ejecución rápida (sin compilar)

El ejecutable Linux compilado está incluido en el repositorio. Solo necesitas clonarlo y correrlo:

```bash
git clone <url-del-repositorio>
cd R2R_Store
./build/R2RStore
```

> Requisito: GPU con soporte **OpenGL 3.3**. Verificar con `glxinfo | grep "OpenGL version"`.

---

## Compilación desde fuente

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

---

## Controles

| Tecla | Acción |
|---|---|
| `W` `A` `S` `D` | Mover cámara |
| Mouse | Apuntar / girar vista |
| `1` | Toggle puertas refrigerador |
| `E` | Toggle puerta de entrada |
| `C` | Toggle animación cafetera |
| `H` | Toggle extracción de helado |
| `R` | Toggle cajón registradora |
| `I` | Toggle ciclo máquina de hielo |
| `ESC` | Salir |

---

## Documentación

| Documento | Descripción |
|---|---|
| [Manual de Usuario](docs/MANUAL_USUARIO.md) | Instalación, controles, tour por la tienda y solución de problemas |
| [Manual Técnico](docs/MANUAL_TECNICO.md) | Arquitectura, animaciones, iluminación, pipeline de materiales y diagramas |

---

## Estructura del proyecto

```
R2R_Store/
├── build/          → Ejecutable compilado (R2RStore)
├── src/            → Código fuente C++ (main.cpp, Mesh.h, Model.h, …)
├── shaders/        → Shaders GLSL (lighting.vert, lighting.frag)
├── models/         → Modelos .obj exportados desde Blender
├── textures/       → Imágenes PNG/JPG de materiales
├── docs/           → Manuales técnico y de usuario
```

---

## Animaciones implementadas

| ID | Nombre | Trigger |
|---|---|---|
| ANIM_01 | Puertas refrigerador | Tecla `1` |
| ANIM_02 | Puerta de entrada | Tecla `E` |
| ANIM_03 | Cámara de seguridad | Automática |
| ANIM_04 | Cajón registradora | Tecla `R` |
| ANIM_05 | Cafetera sirviendo café *(Bézier + smoothstep + glow)* | Tecla `C` |
| ANIM_06 | Extracción de helado *(Bézier cúbica)* | Tecla `H` |
| ANIM_07 | Máquina de hielo *(smoothstep + gravedad newtoniana)* | Tecla `I` |
| ANIM_08 | Letrero neón R2R *(batimiento de ondas)* | Automática |
| ANIM_09 | Lámparas del techo *(frecuencias inconmensurables)* | Automática |

---

## Stack técnico

| Componente | Versión / Uso |
|---|---|
| OpenGL | 3.3 Core Profile |
| C++ | C++17 |
| GLFW | Ventana y eventos |
| GLEW | Carga de extensiones |
| Assimp | Carga de modelos `.obj` |
| GLM | Matemáticas (vec3, mat4, …) |
| stb_image | Carga de texturas PNG/JPG |
| CMake | Sistema de compilación |
