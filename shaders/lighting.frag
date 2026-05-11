#version 330 core

// ── Estructuras ──────────────────────────────────────────────────────────────

struct Material {
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;
    vec3  emissive;
    float shininess;
    float opacity;
    bool  hasTexture;
};

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3  position;
    vec3  ambient;
    vec3  diffuse;
    vec3  specular;
    float constant;
    float linear;
    float quadratic;
};

// ── Inputs / Outputs ─────────────────────────────────────────────────────────

in  vec3 FragPos;
in  vec3 Normal;
in  vec2 TexCoords;
out vec4 FragColor;

// ── Uniforms ─────────────────────────────────────────────────────────────────

uniform Material   material;
uniform DirLight   dirLight;
uniform PointLight pointLights[2];
uniform vec3       viewPos;
uniform sampler2D  texture_diffuse1;
uniform vec3       uBaseColor;        // color sólido para materiales sin textura

// Animaciones de iluminación
uniform float lampIntensity;      // ANIM_08: 0.85 + variación senoidal
uniform float emissiveIntensity;  // ANIM_LETRERO: batimiento de ondas
uniform float alphaOverride;      // ANIM_07: -1 = usar material.opacity

// ── Funciones Blinn-Phong ─────────────────────────────────────────────────────

vec3 CalcDirLight(DirLight light, vec3 norm, vec3 viewDir, vec3 albedo)
{
    vec3 lightDir = normalize(-light.direction);
    float diff    = max(dot(norm, lightDir), 0.0);
    vec3  halfway = normalize(lightDir + viewDir);
    float spec    = pow(max(dot(norm, halfway), 0.0), material.shininess);

    vec3 amb  = light.ambient  * material.ambient  * albedo;
    vec3 dif  = light.diffuse  * diff * material.diffuse * albedo;
    vec3 spc  = light.specular * spec * material.specular;
    return amb + dif + spc;
}

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos,
                    vec3 viewDir, vec3 albedo)
{
    vec3  lightDir   = normalize(light.position - fragPos);
    float diff       = max(dot(norm, lightDir), 0.0);
    vec3  halfway    = normalize(lightDir + viewDir);
    float spec       = pow(max(dot(norm, halfway), 0.0), material.shininess);
    float dist       = length(light.position - fragPos);
    float att        = 1.0 / (light.constant
                            + light.linear    * dist
                            + light.quadratic * dist * dist);

    // lampIntensity escala el brillo de las lámparas del techo
    vec3 amb = light.ambient  * material.ambient  * albedo * att * lampIntensity;
    vec3 dif = light.diffuse  * diff * material.diffuse * albedo * att * lampIntensity;
    vec3 spc = light.specular * spec * material.specular * att * lampIntensity;
    return amb + dif + spc;
}

// ── main ──────────────────────────────────────────────────────────────────────

void main()
{
    vec3 norm    = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 albedo = material.hasTexture
        ? texture(texture_diffuse1, TexCoords).rgb
        : uBaseColor;

    vec3 result = CalcDirLight(dirLight, norm, viewDir, albedo);
    for (int i = 0; i < 2; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, albedo);

    // Emisivo propio del material (Ke del MTL)
    result += material.emissive;

    // ANIM_LETRERO: emisivo animado superpuesto
    result += emissiveIntensity * vec3(0.31, 0.76, 0.97);

    float alpha = (alphaOverride >= 0.0) ? alphaOverride : material.opacity;

    FragColor = vec4(result, alpha);
}
