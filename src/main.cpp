/*
 * R2R Store — Proyecto académico OpenGL 3.3 Core Profile
 *
 * Los archivos OBJ tienen vértices en WORLD SPACE (transformaciones de
 * Blender ya aplicadas en la exportación con Forward:-Z Up:Y).
 * Sistema de coordenadas: OBJ_X = Blender_X,
 *                         OBJ_Y = Blender_Z (altura),
 *                         OBJ_Z = -Blender_Y (profundidad, negado).
 * Las posiciones de ESCENA.md son los orígenes Blender de cada objeto
 * y se usan SOLO como pivotes para animaciones (T·R·T⁻¹).
 *
 * Controles:
 *   WASD / Flechas → mover cámara    Mouse → apuntar
 *   E              → toggle puerta entrada
 *   R              → toggle cajón caja registradora
 *   I              → toggle pausa máquina de hielo (ANIM_07)
 *   ESC            → salir
 */

#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>

#include "Shader.h"
#include "Camera.h"
#include "TextureManager.h"
#include "TextureReport.h"
#include "Model.h"

namespace fs = std::filesystem;

// ─── Prototipos ───────────────────────────────────────────────────────────────
void KeyCallback   (GLFWwindow*, int, int, int, int);
void MouseCallback (GLFWwindow*, double, double);
void DoMovement    ();
void UpdateAnims   (float dt);

// ─── Ventana ─────────────────────────────────────────────────────────────────
const GLuint WIDTH = 1280, HEIGHT = 720;
int SCREEN_W, SCREEN_H;

// ─── Cámara ───────────────────────────────────────────────────────────────────
// Posición inicial: entrada de la tienda — Z=4 (fuera) mirando hacia -Z (dentro)
Camera camera(glm::vec3(0.0f, 1.7f, 4.0f),
              glm::vec3(0.0f, 1.0f, 0.0f),
              -90.0f, 0.0f);

GLfloat lastX = WIDTH  / 2.0f;
GLfloat lastY = HEIGHT / 2.0f;
bool    firstMouse = true;
bool    keys[1024] = {};
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// ─── Estado de animaciones ────────────────────────────────────────────────────
bool  puertaAbierta   = false;   // ANIM_02  tecla E
float puertaAngle     = 0.0f;
bool  puertaPrev      = false;

bool  cajonActivo     = false;   // ANIM_04  tecla R
bool  cajonPrev       = false;

bool  hieloPausado    = false;   // ANIM_07  tecla I
float hieloTimer      = 0.0f;   // segundos [0, 5)
bool  hieloPrev       = false;

// ─── Helpers matemáticos ─────────────────────────────────────────────────────

// Smoothstep cúbico: f(t) = t²(3−2t), t ∈ [0,1]
// Garantiza velocidad 0 en extremos: derivada f'(t)=6t(1−t), máximo en t=0.5
// Evita discontinuidades de velocidad en transiciones de fase
inline float smoothstep(float t) {
    t = glm::clamp(t, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

inline float lerp(float a, float b, float t) { return a + t * (b - a); }

// Bézier cúbica: B(t) = (1-t)³P0 + 3(1-t)²tP1 + 3(1-t)t²P2 + t³P3
// Parametriza la trayectoria de la paleta de helado como arco suave
// P0=inicio, P1/P2=control (arco natural), P3=fin
inline glm::vec3 bezier3(float t,
                          glm::vec3 P0, glm::vec3 P1,
                          glm::vec3 P2, glm::vec3 P3)
{
    float u = 1.0f - t;
    return u*u*u*P0 + 3.0f*u*u*t*P1 + 3.0f*u*t*t*P2 + t*t*t*P3;
}

// Convierte origen Blender (ESCENA.md) a pivote en espacio OBJ/OpenGL:
//   OBJ_X = Blender_X,  OBJ_Y = Blender_Z,  OBJ_Z = -Blender_Y
inline glm::vec3 blenderToOBJ(float bx, float by, float bz) {
    return glm::vec3(bx, bz, -by);
}

// ─── Draw helpers ─────────────────────────────────────────────────────────────

void drawModel(Shader& sh, Model& m, const glm::mat4& t,
               float emissive = 0.0f, float alpha = -1.0f)
{
    sh.setMat4("model", t);
    sh.setFloat("emissiveIntensity", emissive);
    sh.setFloat("alphaOverride",     alpha);
    m.Draw(sh);
}

// Rotación animada alrededor de un pivote en world space
// Patrón: T(pivot) · R · T(-pivot)
inline glm::mat4 pivotRotY(glm::vec3 pivot, float angleDeg) {
    return glm::translate(glm::mat4(1.0f), pivot)
         * glm::rotate(glm::mat4(1.0f), glm::radians(angleDeg), glm::vec3(0,1,0))
         * glm::translate(glm::mat4(1.0f), -pivot);
}

inline glm::mat4 pivotRotX(glm::vec3 pivot, float angleDeg) {
    return glm::translate(glm::mat4(1.0f), pivot)
         * glm::rotate(glm::mat4(1.0f), glm::radians(angleDeg), glm::vec3(1,0,0))
         * glm::translate(glm::mat4(1.0f), -pivot);
}

inline glm::mat4 pivotTranslate(glm::vec3 offset) {
    return glm::translate(glm::mat4(1.0f), offset);
}

// ─── main ─────────────────────────────────────────────────────────────────────
int main(int argc, char* argv[])
{
    // Rutas dinámicas desde el ejecutable (regla: sin rutas absolutas)
    fs::path root    = fs::canonical(fs::path(argv[0]).parent_path()).parent_path();
    std::string mdl  = (root / "models").string()  + "/";
    std::string shd  = (root / "shaders").string() + "/";

    // ── GLFW ─────────────────────────────────────────────────────────────────
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* win = glfwCreateWindow(WIDTH, HEIGHT, "R2R Store", nullptr, nullptr);
    if (!win) { glfwTerminate(); return EXIT_FAILURE; }

    glfwMakeContextCurrent(win);
    glfwGetFramebufferSize(win, &SCREEN_W, &SCREEN_H);
    glfwSetKeyCallback(win, KeyCallback);
    glfwSetCursorPosCallback(win, MouseCallback);
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ── GLEW ─────────────────────────────────────────────────────────────────
    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit()) { return EXIT_FAILURE; }

    glViewport(0, 0, SCREEN_W, SCREEN_H);
    glEnable(GL_DEPTH_TEST);

    Shader shader(shd + "lighting.vert", shd + "lighting.frag");

    // ── Gestor de texturas + relatorio ────────────────────────────────────────
    std::string texDir  = (root / "textures").string();
    std::string repPath = (root / "relatorio_texturas_tienda.txt").string();
    TextureManager texMgr(texDir);
    ObjTexMap      texReport = ParseTextureReport(repPath);

    // ── Modelos estáticos ─────────────────────────────────────────────────────
    // Los OBJ ya tienen vértices en world space. Se renderizan con identidad.
    std::vector<std::unique_ptr<Model>> statics;
    auto addS = [&](const std::string& f) {
        statics.emplace_back(std::make_unique<Model>(mdl + f, texMgr, texReport));
    };

    addS("REF_Estructura_tienda_completa_R2R.obj");
    addS("REF_Piso_completo_R2R.obj");
    addS("REF_Pared_Refrigeradores.obj");
    addS("REF_Pared_Trasera.obj");           // Rot_X=180° ya baked en OBJ
    addS("REF_Trabe_Techo.obj");
    addS("REF_Vidrio_Ventana_1.obj");
    addS("REF_Vidrio_Ventana_2.obj");
    addS("REF_Rack_Bebidas.obj");
    addS("REF_Rack_Cigarros.obj");
    addS("REF_Estante_1.obj");
    addS("REF_Estante_2.obj");
    addS("REF_Mostrador.obj");
    addS("REF_Rack_Rack_Refri_1.obj");
    addS("REF_Rack_Rack_Refri_2.obj");
    addS("REF_Rack_Rack_Refri_3.obj");
    addS("REF_Rack_Rack_Refri_4.obj");
    addS("REF_Rack_Rack_Refri_5.obj");
    addS("REF_Rack_Rack_Refri_6.obj");
    // REF_Rack_Refri_7 no existe en models/
    addS("REF_Rack_Rack_Refri_8.obj");
    addS("REF_Lampara_1.obj");               // ANIM_08 — modelo estático, luz animada
    addS("REF_Lampara_2.obj");
    addS("REF_Folleto_R2R_1.obj"); addS("REF_Folleto_R2R_2.obj");
    addS("REF_Folleto_R2R_3.obj"); addS("REF_Folleto_R2R_4.obj");
    addS("REF_Folleto_R2R_5.obj"); addS("REF_Folleto_R2R_6.obj");
    addS("REF_Folleto_R2R_7.obj"); addS("REF_Folleto_R2R_8.obj");
    addS("REF_Folleto_R2R_9.obj");
    addS("REF_Cartel_Wanted.obj");
    addS("REF_Mueble_Cafetera1.obj");  addS("REF_Mueble_Cafetera2.obj");
    addS("REF_Cafetera1_Body.obj");    addS("REF_Cafetera2_Body.obj");
    addS("REF_Cafetera1_Boquilla_1.obj"); addS("REF_Cafetera1_Boquilla_2.obj");
    addS("REF_Cafetera1_Boquilla_3.obj"); addS("REF_Cafetera2_Boquilla_1.obj");
    addS("REF_Cafetera2_Boquilla_2.obj"); addS("REF_Cafetera2_Boquilla_3.obj");
    addS("REF_Taza_Cafetera1_1.obj"); addS("REF_Taza_Cafetera1_2.obj");
    addS("REF_Taza_Cafetera2_1.obj"); addS("REF_Taza_Cafetera2_2.obj");
    addS("REF_Varios_Cafetera2.obj");
    addS("REF_Helados1_Body.obj");    addS("REF_Helados2_Body.obj");
    addS("REF_Helados2_Tapa.obj");
    addS("REF_Paletas_2.obj");
    addS("REF_Hielo_Body.obj");
    addS("REF_Caja1_Body.obj");       addS("REF_Caja2_Body.obj");
    addS("REF_Camara_Soporte.obj");
    addS("REF_Poste_R2R.obj");
    addS("REF_Basura.obj");

    // ── ANIM_01: Puertas refrigerador ─────────────────────────────────────────
    // Pivotes: origen Blender → OBJ(X, ESCENA_Z, -ESCENA_Y)
    // Todas comparten ESCENA_Z=1.7057 (altura de bisagra) y ESCENA_X=5.7682
    Model mdlR1(mdl+"REF_Refri_Puerta_1.obj",texMgr,texReport),
          mdlR2(mdl+"REF_Refri_Puerta_2.obj",texMgr,texReport),
          mdlR3(mdl+"REF_Refri_Puerta_3.obj",texMgr,texReport),
          mdlR4(mdl+"REF_Refri_Puerta_4.obj",texMgr,texReport),
          mdlR5(mdl+"REF_Refri_Puerta_5.obj",texMgr,texReport),
          mdlR6(mdl+"REF_Refri_Puerta_6.obj",texMgr,texReport),
          mdlR7(mdl+"REF_Refri_Puerta_7.obj",texMgr,texReport),
          mdlR8(mdl+"REF_Refri_Puerta_8.obj",texMgr,texReport);
    Model* mdlRefri[] = {&mdlR1,&mdlR2,&mdlR3,&mdlR4,&mdlR5,&mdlR6,&mdlR7,&mdlR8};

    // ESCENA: (5.7682, refriY[i], 1.7057) → OBJ pivot (5.7682, 1.7057, -refriY[i])
    const float refriY[] = {4.1769f,3.4085f,2.6401f,1.8717f,0.1242f,-0.6442f,-1.4126f,-2.1810f};
    auto refriPivot = [&](int i) {
        return blenderToOBJ(5.7682f, refriY[i], 1.7057f);
    };

    // ── ANIM_02: Puerta de entrada ────────────────────────────────────────────
    // ESCENA Der:(0.3599,-2.7559,1.8163)  Izq:(-1.6287,-2.7753,1.8163)
    Model mdlPuertaDer(mdl+"REF_Puerta_princ_Der.obj",texMgr,texReport);
    Model mdlPuertaIzq(mdl+"REF_Puerta_princ_Izq.obj",texMgr,texReport);
    glm::vec3 pvDer = blenderToOBJ( 0.3599f,-2.7559f, 1.8163f);
    glm::vec3 pvIzq = blenderToOBJ(-1.6287f,-2.7753f, 1.8163f);

    // ── ANIM_03: Cámara de seguridad ──────────────────────────────────────────
    // ESCENA: (-3.8878, 3.6584, 3.5431)
    Model mdlCamaraHead(mdl+"REF_Camara_Head.obj",texMgr,texReport);
    glm::vec3 pvCamara = blenderToOBJ(-3.8878f, 3.6584f, 3.5431f);

    // ── ANIM_04: Cajones registradora ─────────────────────────────────────────
    // ESCENA Cajon1:(-3.5052,-0.3321,1.9916)  Cajon2:(-2.0572,-0.9243,1.9916)
    // Deslizamiento en OBJ +Z (hacia el cliente)
    Model mdlCajon1(mdl+"REF_Caja1_Cajon.obj",texMgr,texReport);
    Model mdlCajon2(mdl+"REF_Caja2_Cajon.obj",texMgr,texReport);

    // ── ANIM_05: Taza de café ─────────────────────────────────────────────────
    // ESCENA TazaCafe:(-1.5279,3.8887,1.6889)
    // Fase 2: nivel sube en OBJ +Y (altura)
    Model mdlTazaCafe(mdl+"REF_Taza_Cafe.obj",texMgr,texReport);
    glm::vec3 pvTaza = blenderToOBJ(-1.5279f, 3.8887f, 1.6889f);

    // ── ANIM_06: Extracción de helado (Bézier cúbica) ─────────────────────────
    // ESCENA Tapa1:(-0.0749,4.0513,1.5434)  Paleta1:(0.3965,3.8992,1.5323)
    Model mdlTapa1  (mdl+"REF_Helados1_Tapa.obj",texMgr,texReport);
    Model mdlPaleta1(mdl+"REF_Paletas_1.obj",    texMgr,texReport);
    glm::vec3 pvTapa1   = blenderToOBJ(-0.0749f, 4.0513f, 1.5434f);
    glm::vec3 pvPaleta1 = blenderToOBJ( 0.3965f, 3.8992f, 1.5323f);
    // Puntos de control Bézier (espacio OBJ/world):
    // P0=dentro del contenedor, P3=fuera (hacia el cliente)
    glm::vec3 bezP0 = pvPaleta1;
    glm::vec3 bezP1 = pvPaleta1 + glm::vec3(0.0f,  0.25f,  0.0f);
    glm::vec3 bezP2 = pvPaleta1 + glm::vec3(0.15f, 0.45f,  0.2f);
    glm::vec3 bezP3 = pvPaleta1 + glm::vec3(0.15f, 0.45f,  0.5f);

    // ── ANIM_07: Máquina de hielo ─────────────────────────────────────────────
    // ESCENA Puerta:(-3.2104,2.4565,1.8957)  Bolsa:(-3.4436,2.0144,1.8423)
    Model mdlHieloPuerta(mdl+"REF_Hielo_Puerta.obj",texMgr,texReport);
    Model mdlHieloBolsa (mdl+"REF_Hielo_Bolsa.obj", texMgr,texReport);
    glm::vec3 pvHieloPuerta = blenderToOBJ(-3.2104f, 2.4565f, 1.8957f);
    glm::vec3 bolsaInit     = blenderToOBJ(-3.4436f, 2.0144f, 1.8423f);
    // Posición final: la bolsa sale hacia afuera (+Z) y cae ligeramente (-Y)
    glm::vec3 bolsaFinal    = bolsaInit + glm::vec3(0.0f, -0.15f, 0.6f);

    // ── ANIM_LETRERO: Logos R2R emisivos ─────────────────────────────────────
    Model mdlLogoEntrada(mdl+"REF_Logo_Tienda_Entrada_Principal.obj",texMgr,texReport);
    Model mdlLogoPoste  (mdl+"REF_Logo_poste_R2R.obj",              texMgr,texReport);

    // ── Proyección ────────────────────────────────────────────────────────────
    glm::mat4 proj = glm::perspective(
        glm::radians(camera.GetZoom()),
        (float)SCREEN_W / (float)SCREEN_H, 0.1f, 100.0f);

    // ── Bucle principal ───────────────────────────────────────────────────────
    const glm::mat4 I(1.0f);   // identidad para objetos sin transformación

    while (!glfwWindowShouldClose(win))
    {
        float t = (float)glfwGetTime();
        deltaTime = t - lastFrame;
        lastFrame = t;

        glfwPollEvents();
        DoMovement();

        // Toggles con detección de flanco
        auto toggleOn = [&](int key, bool& prev) -> bool {
            bool cur = keys[key];
            bool trig = cur && !prev;
            prev = cur;
            return trig;
        };
        if (toggleOn(GLFW_KEY_E, puertaPrev)) puertaAbierta = !puertaAbierta;
        if (toggleOn(GLFW_KEY_R, cajonPrev))  cajonActivo   = !cajonActivo;
        if (toggleOn(GLFW_KEY_I, hieloPrev))  hieloPausado  = !hieloPausado;

        UpdateAnims(deltaTime);

        // ── Clear ─────────────────────────────────────────────────────────────
        glClearColor(0.05f, 0.07f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();

        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view",       view);
        shader.setMat4("projection", proj);
        shader.setVec3("viewPos",    camera.GetPosition());

        // ── Luz direccional (sol exterior) ────────────────────────────────────
        shader.setVec3("dirLight.direction", glm::vec3(-0.5f,-1.0f,-0.3f));
        shader.setVec3("dirLight.ambient",   glm::vec3(0.05f, 0.07f, 0.10f));
        shader.setVec3("dirLight.diffuse",   glm::vec3(1.0f,  0.98f, 0.95f));
        shader.setVec3("dirLight.specular",  glm::vec3(0.3f,  0.3f,  0.3f));

        // ── ANIM_08: Lámparas del techo — parpadeo suave ─────────────────────
        // I(t) = 0.85 + 0.1·sin(1.3t) + 0.05·sin(3.7t)
        // Superposición de dos frecuencias → variación no periódica suave
        float lampI = 0.85f + 0.10f*sinf(1.3f*t) + 0.05f*sinf(3.7f*t);
        shader.setFloat("lampIntensity", lampI);

        const glm::vec3 lampClr(1.0f, 0.97f, 0.88f);  // blanco cálido
        // Lámparas ya en world space (OBJ_Y = ESCENA_Z, OBJ_Z = -ESCENA_Y)
        // ESCENA Lampara1:(-0.7389, 0.4617, 3.6875) → OBJ(-0.7389, 3.6875, -0.4617)
        // ESCENA Lampara2:( 2.9418, 0.4617, 3.6875) → OBJ( 2.9418, 3.6875, -0.4617)
        glm::vec3 lPos0 = blenderToOBJ(-0.7389f, 0.4617f, 3.6875f);
        glm::vec3 lPos1 = blenderToOBJ( 2.9418f, 0.4617f, 3.6875f);

        shader.setVec3 ("pointLights[0].position",  lPos0);
        shader.setVec3 ("pointLights[0].ambient",   lampClr * 0.05f);
        shader.setVec3 ("pointLights[0].diffuse",   lampClr);
        shader.setVec3 ("pointLights[0].specular",  lampClr * 0.3f);
        shader.setFloat("pointLights[0].constant",  1.0f);
        shader.setFloat("pointLights[0].linear",    0.07f);
        shader.setFloat("pointLights[0].quadratic", 0.017f);

        shader.setVec3 ("pointLights[1].position",  lPos1);
        shader.setVec3 ("pointLights[1].ambient",   lampClr * 0.05f);
        shader.setVec3 ("pointLights[1].diffuse",   lampClr);
        shader.setVec3 ("pointLights[1].specular",  lampClr * 0.3f);
        shader.setFloat("pointLights[1].constant",  1.0f);
        shader.setFloat("pointLights[1].linear",    0.07f);
        shader.setFloat("pointLights[1].quadratic", 0.017f);

        // ─────────────────────────────────────────────────────────────────────
        // Pre-computar estado de todas las animaciones (usado en ambos passes)
        // ─────────────────────────────────────────────────────────────────────

        // ANIM_01: puertas refri — angle = 45°·(1+sin(t·0.8)) ∈ [0°,90°]
        float refriAngle = 45.0f * (1.0f + sinf(t * 0.8f));

        // ANIM_03: cámara — paneo oscilante ±60°
        float camaraAngle = 60.0f * sinf(t * 0.5f);

        // ANIM_04: cajones — deslizamiento Z cuando activo
        float offset04 = cajonActivo ? 0.15f * (1.0f + sinf(t * 1.2f)) * 0.5f : 0.0f;

        // ANIM_05: taza café — sube en Y con smoothstep cúbico
        // Fase 1 (0.0→0.3): espera  Fase 2 (0.3→0.8): sube  Fase 3 (0.8→1.0): llena
        float rise05 = 0.0f;
        {
            float ct = fmodf(t, 5.0f) / 5.0f;
            if      (ct >= 0.3f && ct <= 0.8f) rise05 = 0.06f * smoothstep((ct - 0.3f) / 0.5f);
            else if (ct >  0.8f)               rise05 = 0.06f;
        }

        // ANIM_06: helados — tapa (rot X) + paleta (curva Bézier cúbica)
        // B(t)=(1-t)³P0+3(1-t)²tP1+3(1-t)t²P2+t³P3  con smoothstep por fase
        float tapaAngle06  = 0.0f;
        glm::vec3 paletaPos06 = bezP0;
        {
            float ht = fmodf(t, 8.0f) / 8.0f;
            if (ht < 0.33f) {
                tapaAngle06 = -45.0f * smoothstep(ht / 0.33f);
            } else if (ht < 0.66f) {
                float tL = (ht - 0.33f) / 0.33f;
                tapaAngle06 = -45.0f;
                paletaPos06 = bezier3(smoothstep(tL), bezP0, bezP1, bezP2, bezP3);
            } else {
                float tL = (ht - 0.66f) / 0.34f;
                tapaAngle06 = -45.0f * (1.0f - smoothstep(tL));
                paletaPos06 = bezier3(1.0f - smoothstep(tL), bezP0, bezP1, bezP2, bezP3);
            }
        }

        // ANIM_LETRERO: batimiento de ondas E(t) = 0.5(1+sin(2π·0.8·t)) + 0.3(1+sin(2π·2.1·t))
        float emissive07 = glm::clamp(
            0.5f*(1.0f + sinf(2.0f*glm::pi<float>()*0.8f*t)) +
            0.3f*(1.0f + sinf(2.0f*glm::pi<float>()*2.1f*t)),
            0.0f, 1.0f);

        // ANIM_07: máquina de hielo — compuerta + bolsa
        // Fases: apertura(0→0.25), bolsa sale(0.25→0.70), reposo(0.70→0.85), cierre(0.85→1.0)
        // Smoothstep cúbico: f(t)=t²(3-2t) → velocidad 0 en extremos (arranque/frenado suave)
        float nt07 = hieloTimer / 5.0f;
        float puertaAng07 = 0.0f;
        if      (nt07 <= 0.25f) puertaAng07 = -90.0f * smoothstep(nt07 / 0.25f);
        else if (nt07 <= 0.85f) puertaAng07 = -90.0f;
        else                    puertaAng07 = -90.0f * (1.0f - smoothstep((nt07 - 0.85f) / 0.15f));

        // Posición bolsa (Δy con gravedad: ½·g·tL² — cinemática newtoniana)
        glm::vec3 bPos07 = bolsaInit;
        if (nt07 > 0.25f) {
            if (nt07 <= 0.70f) {
                float tL   = (nt07 - 0.25f) / 0.45f;
                float ease = smoothstep(tL);
                bPos07.z = lerp(bolsaInit.z, bolsaFinal.z, ease);
                bPos07.y = lerp(bolsaInit.y, bolsaFinal.y, ease);
                bPos07.y += 0.5f * (-9.8f * 0.015f) * tL * tL;
            } else {
                bPos07 = bolsaFinal;
            }
        }

        // ─────────────────────────────────────────────────────────────────────
        // Helper: configura uniforms y despacha opaco o transparente
        // ─────────────────────────────────────────────────────────────────────
        auto draw = [&](bool trans, Model& m, const glm::mat4& xf,
                        float em = 0.0f, float al = -1.0f)
        {
            shader.setMat4  ("model",             xf);
            shader.setFloat ("emissiveIntensity", em);
            shader.setFloat ("alphaOverride",     al);
            if (trans) m.DrawTransparent(shader);
            else       m.DrawOpaque(shader);
        };

        // Lambda que agrupa todos los draw-calls de la escena en un pase
        // 'trans=false' → meshes opacos   'trans=true' → meshes transparentes
        auto renderScene = [&](bool trans)
        {
            // Estáticos (world space baked en OBJ → identidad)
            for (auto& s : statics)
                draw(trans, *s, I);

            // ANIM_01 — puertas refrigerador (rot Y alrededor de bisagra)
            for (int i = 0; i < 8; i++)
                draw(trans, *mdlRefri[i], pivotRotY(refriPivot(i), refriAngle));

            // ANIM_02 — puerta de entrada (toggle E)
            draw(trans, mdlPuertaDer, pivotRotY(pvDer, -puertaAngle));
            draw(trans, mdlPuertaIzq, pivotRotY(pvIzq,  puertaAngle));

            // ANIM_03 — cámara de seguridad
            draw(trans, mdlCamaraHead, pivotRotY(pvCamara, camaraAngle));

            // ANIM_04 — cajones registradora (toggle R)
            draw(trans, mdlCajon1, pivotTranslate(glm::vec3(0, 0, offset04)));
            draw(trans, mdlCajon2, pivotTranslate(glm::vec3(0, 0, offset04)));

            // ANIM_05 — taza de café
            draw(trans, mdlTazaCafe, pivotTranslate(glm::vec3(0, rise05, 0)));

            // ANIM_06 — helados: tapa y paleta
            draw(trans, mdlTapa1,   pivotRotX(pvTapa1, tapaAngle06));
            draw(trans, mdlPaleta1, pivotTranslate(paletaPos06 - pvPaleta1));

            // ANIM_LETRERO — logos R2R neón pulsante
            draw(trans, mdlLogoEntrada, I, emissive07);
            draw(trans, mdlLogoPoste,   I, emissive07);

            // ANIM_07 — compuerta de hielo (la bolsa se maneja en PASS 3)
            draw(trans, mdlHieloPuerta, pivotRotX(pvHieloPuerta, puertaAng07));
        };

        // ═════════════════════════════════════════════════════════════════════
        // PASS 1 — OPACOS: depth R/W, blend OFF
        // ═════════════════════════════════════════════════════════════════════
        renderScene(false);

        // ═════════════════════════════════════════════════════════════════════
        // PASS 2 — TRANSPARENTES: depth lectura-only, blend ON
        // Objetos con vidrio: ventanas, puertas, tapas helados, puertas refri
        // glDepthMask(GL_FALSE) evita que el vidrio bloquee objetos detrás
        // ═════════════════════════════════════════════════════════════════════
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        renderScene(true);

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        // ═════════════════════════════════════════════════════════════════════
        // PASS 3 — Bolsa de hielo semitransparente (alpha=0.85 por override)
        // Debe ir AL FINAL: se renderiza sobre todo lo ya visible
        // ═════════════════════════════════════════════════════════════════════
        if (nt07 > 0.25f) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            drawModel(shader, mdlHieloBolsa,
                      pivotTranslate(bPos07 - bolsaInit),
                      0.0f, 0.85f);

            glDisable(GL_BLEND);
        }

        glfwSwapBuffers(win);
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}

// ─── UpdateAnims ─────────────────────────────────────────────────────────────
void UpdateAnims(float dt)
{
    // ANIM_02: puerta → lerp suave hacia target (120°/s)
    float target = puertaAbierta ? 90.0f : 0.0f;
    const float spd = 120.0f;
    if (puertaAngle < target)
        puertaAngle = glm::min(puertaAngle + spd * dt, target);
    else if (puertaAngle > target)
        puertaAngle = glm::max(puertaAngle - spd * dt, target);

    // ANIM_07: timer cíclico (si no está pausado)
    if (!hieloPausado) {
        hieloTimer += dt;
        if (hieloTimer >= 5.0f) hieloTimer -= 5.0f;
    }
}

// ─── Input ───────────────────────────────────────────────────────────────────
void DoMovement()
{
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])    camera.ProcessKeyboard(FORWARD,  deltaTime);
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])  camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])  camera.ProcessKeyboard(LEFT,     deltaTime);
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) camera.ProcessKeyboard(RIGHT,    deltaTime);
}

void KeyCallback(GLFWwindow* win, int key, int, int action, int)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(win, GL_TRUE);
    if (key >= 0 && key < 1024) {
        if      (action == GLFW_PRESS)   keys[key] = true;
        else if (action == GLFW_RELEASE) keys[key] = false;
    }
}

void MouseCallback(GLFWwindow*, double xPos, double yPos)
{
    if (firstMouse) { lastX=(float)xPos; lastY=(float)yPos; firstMouse=false; }
    float xOff = (float)xPos - lastX;
    float yOff = lastY - (float)yPos;
    lastX = (float)xPos; lastY = (float)yPos;
    camera.ProcessMouseMovement(xOff, yOff);
}
