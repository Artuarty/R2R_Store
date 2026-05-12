# Manual de Usuario — R2R Store
## Entorno Virtual 3D Interactivo

**Proyecto:** R2R Store — Tienda de Conveniencia Virtual  
**Tecnología:** OpenGL 3.3 Core Profile / C++17  
**Versión:** 1.0 — Mayo 2026

---

## 1. Requisitos del Sistema

| Componente | Mínimo requerido |
|---|---|
| Sistema operativo | Linux (Ubuntu 20.04+) / Windows 10+ / macOS 11+ |
| GPU | Compatible con OpenGL 3.3 Core Profile |
| RAM | 512 MB disponibles |
| Almacenamiento | 200 MB libres |
| Dependencias | GLFW3, GLEW, Assimp, GLM (instaladas vía CMake) |

---

## 2. Instalación y Ejecución

### 2.1 Ejecución directa (recomendado)

El repositorio incluye el ejecutable Linux ya compilado en `build/R2RStore`. No es necesario compilar para probar el proyecto por primera vez:

```bash
# 1. Clonar el repositorio
git clone <url-del-repositorio>
cd R2R_Store

# 2. Ejecutar directamente
./build/R2RStore
```

> **Nota:** El ejecutable es un binario ELF 64-bit para Linux x86-64. Requiere las librerías compartidas `libGL`, `libGLEW`, `libglfw` y `libassimp` instaladas en el sistema.  
> En Ubuntu/Debian: `sudo apt install libglew-dev libglfw3 libassimp-dev`

### 2.2 Compilación desde código fuente (opcional)

Si deseas recompilar o modificar el proyecto:

```bash
# Desde la raíz del proyecto
mkdir -p build && cd build

# Configurar con CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Compilar (usa todos los núcleos disponibles)
make -j$(nproc)
```

### 2.3 Ejecutar el programa

```bash
# Desde la raíz del proyecto
./build/R2RStore
```

> **Importante:** El ejecutable debe lanzarse desde la raíz del proyecto o usando la ruta relativa `./build/R2RStore`. Las rutas a modelos, texturas y shaders se calculan automáticamente desde la ubicación del ejecutable — no mover el binario fuera de `build/`.

---

## 3. Controles

### 3.1 Movimiento de cámara

| Tecla | Acción |
|---|---|
| `W` / `↑` | Avanzar hacia el interior de la tienda |
| `S` / `↓` | Retroceder hacia la entrada |
| `A` / `←` | Desplazarse a la izquierda |
| `D` / `→` | Desplazarse a la derecha |
| **Mouse** | Girar la cámara (apuntar en cualquier dirección) |

> La cámara inicia en la **entrada principal de la tienda**, a 1.7 metros de altura (altura de ojos), mirando hacia el interior.

### 3.2 Interacciones con objetos

| Tecla | Acción | Descripción |
|---|---|---|
| `1` | **Toggle puertas refrigerador** | Abre o cierra las 8 puertas del refrigerador (animación suave 60°/s) |
| `E` | **Toggle puerta de entrada** | Abre o cierra las puertas principales abatiéndose en 90° (120°/s) |
| `C` | **Toggle cafetera** | Inicia o detiene el ciclo de servido de café en la taza |
| `H` | **Toggle helados** | Inicia o detiene la extracción de paleta del contenedor de helados |
| `R` | **Toggle cajón registradora** | Extiende o retrae el cajón de las cajas registradoras |
| `I` | **Toggle máquina de hielo** | Inicia o detiene el ciclo de dispensado de bolsa de hielo (5 s) |
| `ESC` | **Salir** | Cierra el programa |

### 3.3 Captura de mouse

Al iniciar el programa el cursor queda capturado dentro de la ventana (modo FPS). Para recuperar el control del escritorio, presiona `ESC`.

---

## 4. Tour por la Tienda

### 4.1 Vista exterior
Al iniciar, te encuentras frente a la **fachada R2R Store**:
- El logo R2R sobre la entrada pulsa con luz de neón azul eléctrico (animación automática)
- Las ventanas frontales muestran el interior de la tienda con efecto de vidrio
- La cámara de seguridad en la esquina superior izquierda gira automáticamente

### 4.2 Entrada principal
- Presiona `E` para **abrir las puertas** de entrada (dos hojas abatibles)
- Las puertas se abren con animación suave y pueden cerrarse presionando `E` nuevamente

### 4.3 Interior — zona de refrigeradores (pared derecha)
- Presiona `1` para **abrir o cerrar** las 8 puertas de refrigerador simultáneamente
- Las puertas se desplazan suavemente a 60°/s; presiona `1` de nuevo para cerrarlas
- Los estantes detrás del vidrio contienen productos variados

### 4.4 Interior — zona de cafeteras (pared trasera izquierda)
- Presiona `C` para **iniciar el servido de café**
- Observa la animación de 3 fases: la taza se ilumina con glow dorado, el nivel de café sube 12 cm con movimiento suave (smoothstep), y el glow desvanece al llenarse
- Presiona `C` de nuevo para detener y reiniciar el ciclo

### 4.5 Interior — zona de helados (pared trasera derecha)
- Presiona `H` para **iniciar la extracción de paleta** del contenedor de helados
- La tapa del contenedor se abre, la paleta sube describiendo un arco Bézier suave, luego ambas regresan a su posición original
- Presiona `H` de nuevo para detener y reiniciar el ciclo

### 4.6 Máquina de hielo (pared izquierda)
- Presiona `I` para **iniciar el ciclo** de dispensado (dura 5 segundos):
  1. La **compuerta cae** hacia el interior de la tienda
  2. Una **bolsa de hielo semitransparente** (alpha = 0.85) sale deslizándose hacia el frente con efecto de gravedad
  3. La bolsa descansa visible un instante, luego la compuerta se cierra
- Presiona `I` de nuevo para detener el ciclo en cualquier momento

### 4.7 Mostradores y cajas registradoras
- Presiona `R` para abrir los **cajones** de las dos cajas registradoras
- Los cajones se deslizan hacia el cliente con movimiento sinusoidal suave

---

## 5. Efectos Visuales

| Efecto | Descripción |
|---|---|
| **Iluminación Blinn-Phong** | 1 luz direccional (sol exterior) + 3 luces puntuales (2 lámparas de techo + 1 luz cálida de cafeteras) |
| **Parpadeo de lámparas** | Las lámparas del techo varían suavemente su intensidad con dos frecuencias superpuestas |
| **Neón pulsante** | El logo R2R late con patrón de batimiento de dos ondas senoidales (azul eléctrico) |
| **Glow de café** | La taza emite un glow dorado-marrón cálido mientras se sirve el café (tecla C) |
| **Vidrio transparente** | Ventanas y puertas con efecto de vidrio (15% opacidad de tinte) |
| **Bolsa semitransparente** | Bolsa de hielo con alpha = 0.85, renderizada con blending correcto sobre toda la escena |
| **Texturas Blender** | Todos los materiales usan sus colores y texturas originales de Blender |

---

## 6. Solución de Problemas

| Problema | Solución |
|---|---|
| Pantalla negra al iniciar | Verificar que la GPU soporte OpenGL 3.3. Ejecutar `glxinfo | grep "OpenGL version"` |
| Modelos no aparecen | Ejecutar el binario como `./build/R2RStore`, no como `./R2RStore` desde otra ruta |
| El mouse no responde | El cursor está capturado. Mover el mouse dentro de la ventana |
| Rendimiento bajo | Cerrar aplicaciones en segundo plano; el proyecto no requiere GPU dedicada pero se beneficia de ella |

---

## 7. Créditos

- **Modelado 3D:** Blender — exportación Forward:-Z Up:Y
- **Motor gráfico:** OpenGL 3.3 Core Profile
- **Código base:** ejemplos del curso de Gráficas por Computadora
- **Fecha de entrega:** Mayo 2026
