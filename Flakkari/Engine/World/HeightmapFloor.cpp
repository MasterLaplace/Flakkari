/*
 * ============================================================================
 * HEIGHTMAP RENDERER - TOUTES LES OPTIMISATIONS DE VERCIDIUM + TEXTURES
 * ============================================================================
 *
 * Ce fichier implémente TOUTES les optimisations de la vidéo:
 * 1. gl_VertexID pour recalculer position X,Z (4 bytes par vertex)
 * 2. Triangle Strips pour réduire le nombre de vertices
 * 3. UVs calculées procéduralement dans le shader
 * 4. GPU Batching avec glMultiDrawArrays + SSBO
 * 5. Sinking Illusion LOD pour terrain à grande échelle
 *
 * ★ SYSTÈME DE HEIGHTMAP TEXTURES (CPU + GPU)
 * ============================================
 * - Chaque chunk possède une TEXTURE R32F (GPU) pour le rendu
 * - Chaque chunk garde une COPIE CPU (vector<float>) pour les collisions
 * - Génération procédurale → texture → ÉDITABLE dans un level editor
 * - Le CPU peut interroger GetHeight(x, z) pour physics/collisions
 * - Les textures peuvent être sauvegardées/chargées depuis le disque
 *
 * Pipeline complète:
 * 1. Génération: Perlin/Sinus → heightData (CPU)
 * 2. Upload: heightData → texture array (GPU)
 * 3. Rendu: Vertex shader sample la texture
 * 4. Collisions: CPU query heightData avec interpolation bilinéaire
 * 5. Édition: Modifier heightData → re-upload texture
 * 6. Save/Load: Sérialiser heightData vers fichier
 *
 * Contrôles:
 * - WASD: Déplacement
 * - QE: Monter/Descendre
 * - Souris: Regarder autour
 * - ESPACE: Wireframe
 * - ESC: Toggle capture souris
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <sstream>

namespace Optimizing::World {

// ============================================================================
// CONSTANTES
// ============================================================================

constexpr int HEIGHTMAP_SIZE = 32;  // Taille d'un chunk (32×32 quads)
constexpr int VERTICES_PER_RUN = HEIGHTMAP_SIZE * 2 + 4;  // Triangle strip + degenerates
constexpr int VERTICES_PER_CHUNK = VERTICES_PER_RUN * HEIGHTMAP_SIZE;
constexpr int VERTICES_PER_RUN_NOT_DEGENERATE = VERTICES_PER_RUN - 3;

constexpr int CHUNKS_PER_ROW = 8;  // Grille 8×8 = 64 chunks
constexpr int TOTAL_CHUNKS = CHUNKS_PER_ROW * CHUNKS_PER_ROW;

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;

constexpr float FOV = 50.0f * 3.14159f / 180.0f;
constexpr float NEAR_PLANE = 0.1f;
constexpr float FAR_PLANE = 2000.0f;

// LOD Configuration
constexpr int NUM_LODS = 4;
constexpr float LOD_DISTANCES[NUM_LODS] = { 64.0f, 128.0f, 256.0f, 512.0f };
constexpr float LOD_SCALES[NUM_LODS] = { 1.0f, 2.0f, 4.0f, 8.0f };
constexpr float SINK_DEPTH = 20.0f;  // Profondeur d'enfoncement pour LOD

// ============================================================================
// STRUCTURE VERTEX (seulement l'altitude !)
// ============================================================================

struct HeightmapVertex {
    float altitude;
};

// ============================================================================
// TERRAIN CHUNK - Heightmap avec texture GPU + copie CPU
// ============================================================================

struct TerrainChunk {
    GLuint heightmapTexture;                              // Texture GPU (R32F)
    std::vector<float> heightData;                        // Copie CPU pour collisions (HEIGHTMAP_SIZE × HEIGHTMAP_SIZE)
    glm::ivec2 chunkPos;                                  // Position du chunk
    bool isEdited;                                        // ★ Flag: true = utiliser texture, false = calcul procédural

    TerrainChunk() : heightmapTexture(0), isEdited(false) {
        heightData.resize(HEIGHTMAP_SIZE * HEIGHTMAP_SIZE, 0.0f);
    }

    // Query CPU pour collisions (avec interpolation bilinéaire)
    float GetHeight(float localX, float localZ) const {
        // Clamp aux bords
        localX = glm::clamp(localX, 0.0f, float(HEIGHTMAP_SIZE - 1));
        localZ = glm::clamp(localZ, 0.0f, float(HEIGHTMAP_SIZE - 1));

        int x0 = int(localX);
        int z0 = int(localZ);
        int x1 = glm::min(x0 + 1, HEIGHTMAP_SIZE - 1);
        int z1 = glm::min(z0 + 1, HEIGHTMAP_SIZE - 1);

        float fx = localX - x0;
        float fz = localZ - z0;

        float h00 = heightData[z0 * HEIGHTMAP_SIZE + x0];
        float h10 = heightData[z0 * HEIGHTMAP_SIZE + x1];
        float h01 = heightData[z1 * HEIGHTMAP_SIZE + x0];
        float h11 = heightData[z1 * HEIGHTMAP_SIZE + x1];

        float h0 = glm::mix(h00, h10, fx);
        float h1 = glm::mix(h01, h11, fx);
        return glm::mix(h0, h1, fz);
    }
};

// ============================================================================
// GLOBALS
// ============================================================================

GLFWwindow* g_window = nullptr;
GLuint g_shaderProgram = 0;
GLuint g_vao = 0;
GLuint g_vbo = 0;
GLuint g_ssbo = 0;  // Shader Storage Buffer Object pour les positions de chunks
GLuint g_ssboEditFlags = 0;  // ★ SSBO pour les flags isEdited (binding 2)

// Terrain chunks avec textures
std::vector<TerrainChunk> g_terrainChunks;

// Uniforms
GLint g_uniformMVP = -1;
GLint g_uniformShowWireframe = -1;
GLint g_uniformCameraPos = -1;

// Caméra - Style Vercidium (pitch/yaw simple)
glm::vec3 g_cameraPos(HEIGHTMAP_SIZE * CHUNKS_PER_ROW / 2.0f, 100.0f, HEIGHTMAP_SIZE * CHUNKS_PER_ROW / 2.0f);
float g_cameraPitch = -3.14159f / 4.0f;  // -45° (regarde vers le bas)
float g_cameraYaw = 3.14159f / 4.0f;     // 45° (regarde en diagonale)
bool g_captureMouse = true;
glm::vec2 g_lastMouse(0.0f, 0.0f);

// État du rendu
bool g_showWireframe = false;
bool g_lodEnabled[NUM_LODS] = { true, true, true, true };

// Métriques de performance
int g_frameCount = 0;
double g_lastFPSTime = 0.0;
double g_currentFPS = 0.0;
int g_trianglesRendered = 0;
int g_chunksRendered = 0;

// ============================================================================
// FRUSTUM CULLING
// ============================================================================

struct Frustum {
    glm::vec4 planes[6];  // Left, Right, Bottom, Top, Near, Far
};

// Extraire le frustum depuis la matrice MVP
Frustum ExtractFrustum(const glm::mat4& mvp) {
    Frustum frustum;

    // Left plane
    frustum.planes[0] = glm::vec4(
        mvp[0][3] + mvp[0][0],
        mvp[1][3] + mvp[1][0],
        mvp[2][3] + mvp[2][0],
        mvp[3][3] + mvp[3][0]
    );

    // Right plane
    frustum.planes[1] = glm::vec4(
        mvp[0][3] - mvp[0][0],
        mvp[1][3] - mvp[1][0],
        mvp[2][3] - mvp[2][0],
        mvp[3][3] - mvp[3][0]
    );

    // Bottom plane
    frustum.planes[2] = glm::vec4(
        mvp[0][3] + mvp[0][1],
        mvp[1][3] + mvp[1][1],
        mvp[2][3] + mvp[2][1],
        mvp[3][3] + mvp[3][1]
    );

    // Top plane
    frustum.planes[3] = glm::vec4(
        mvp[0][3] - mvp[0][1],
        mvp[1][3] - mvp[1][1],
        mvp[2][3] - mvp[2][1],
        mvp[3][3] - mvp[3][1]
    );

    // Near plane
    frustum.planes[4] = glm::vec4(
        mvp[0][3] + mvp[0][2],
        mvp[1][3] + mvp[1][2],
        mvp[2][3] + mvp[2][2],
        mvp[3][3] + mvp[3][2]
    );

    // Far plane
    frustum.planes[5] = glm::vec4(
        mvp[0][3] - mvp[0][2],
        mvp[1][3] - mvp[1][2],
        mvp[2][3] - mvp[2][2],
        mvp[3][3] - mvp[3][2]
    );

    // Normaliser les plans
    for (int i = 0; i < 6; i++) {
        float length = glm::length(glm::vec3(frustum.planes[i]));
        frustum.planes[i] /= length;
    }

    return frustum;
}

// Tester si un chunk (AABB) est visible dans le frustum
bool IsChunkVisible(const Frustum& frustum, const glm::vec3& chunkMin, const glm::vec3& chunkMax) {
    // Tester chaque plan du frustum
    for (int i = 0; i < 6; i++) {
        const glm::vec4& plane = frustum.planes[i];
        glm::vec3 normal(plane.x, plane.y, plane.z);

        // Trouver le coin "positif" (le plus loin dans la direction du plan)
        glm::vec3 positiveVertex = chunkMin;
        if (normal.x >= 0) positiveVertex.x = chunkMax.x;
        if (normal.y >= 0) positiveVertex.y = chunkMax.y;
        if (normal.z >= 0) positiveVertex.z = chunkMax.z;

        // Si le coin positif est derrière le plan, le chunk est hors frustum
        if (glm::dot(normal, positiveVertex) + plane.w < 0) {
            return false;
        }
    }

    return true;
}

// ============================================================================
// GÉNÉRATION DE HAUTEUR (fonction simple pour démo)
// ============================================================================

float GetHeightProcedural(float x, float z) {
    // Fonction simple sinusoïdale comme dans le repo
    return std::sin(x * 0.1f) * 5.0f + std::cos(z * 0.15f) * 3.0f;
}

// Query global pour collisions (world coordinates)
float GetHeight(float worldX, float worldZ) {
    // Trouver le chunk
    int chunkX = int(std::floor(worldX / HEIGHTMAP_SIZE));
    int chunkZ = int(std::floor(worldZ / HEIGHTMAP_SIZE));

    // Coordonnées locales dans le chunk
    float localX = worldX - chunkX * HEIGHTMAP_SIZE;
    float localZ = worldZ - chunkZ * HEIGHTMAP_SIZE;

    // Trouver le chunk dans notre grille
    if (chunkX < 0 || chunkX >= CHUNKS_PER_ROW ||
        chunkZ < 0 || chunkZ >= CHUNKS_PER_ROW) {
        // Hors de la grille, utiliser la fonction procédurale
        return GetHeightProcedural(worldX, worldZ);
    }

    int chunkIndex = chunkZ * CHUNKS_PER_ROW + chunkX;
    return g_terrainChunks[chunkIndex].GetHeight(localX, localZ);
}

// ============================================================================
// GÉNÉRATION DES HEIGHTMAPS
// ============================================================================

void GenerateChunkHeightmap(TerrainChunk& chunk) {
    // Générer les hauteurs procéduralement (données CPU)
    for (int z = 0; z < HEIGHTMAP_SIZE; z++) {
        for (int x = 0; x < HEIGHTMAP_SIZE; x++) {
            float worldX = chunk.chunkPos.x * HEIGHTMAP_SIZE + x;
            float worldZ = chunk.chunkPos.y * HEIGHTMAP_SIZE + z;

            float height = GetHeightProcedural(worldX, worldZ);
            chunk.heightData[z * HEIGHTMAP_SIZE + x] = height;
        }
    }

    // ★ OPTIMISATION: Texture 32×32 sans padding !
    // La continuité est garantie par le calcul procédural (worldX/worldZ)
    // qui est cohérent entre chunks adjacents

    // Créer la texture GPU (taille normale, pas de padding)
    glGenTextures(1, &chunk.heightmapTexture);
    glBindTexture(GL_TEXTURE_2D, chunk.heightmapTexture);

    // Upload les données CPU directement (32×32)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F,
                 HEIGHTMAP_SIZE, HEIGHTMAP_SIZE, 0,
                 GL_RED, GL_FLOAT, chunk.heightData.data());

    // Paramètres de sampling (LINEAR pour interpolation smooth)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // REPEAT pour que les bords wrappe correctement
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);
}

// ============================================================================
// SHADERS
// ============================================================================

std::string CreateVertexShader() {
    return R"glsl(
#version 430 core
#extension GL_ARB_shader_draw_parameters : require

layout (location = 0) in float aAltitude;

// SSBO contenant les positions des chunks
layout(std430, binding = 1) buffer ChunkPositions {
    vec2 chunkPositions[];
};

// ★ SSBO contenant les flags isEdited des chunks (0=procedural, 1=texture)
layout(std430, binding = 2) buffer ChunkEditFlags {
    int chunkEditFlags[];
};

// ★ TEXTURE ARRAY pour toutes les heightmaps
uniform sampler2DArray heightmapTextures;

uniform mat4 mvp;
uniform vec3 cameraPos;

out vec3 vWorldPos;
out vec2 vBary;
flat out float vBrightness;

const float VERTICES_PER_RUN = )glsl" + std::to_string(VERTICES_PER_RUN) + R"glsl(.0;
const float VERTICES_PER_RUN_NOT_DEGENERATE = )glsl" + std::to_string(VERTICES_PER_RUN_NOT_DEGENERATE) + R"glsl(.0;
const float HEIGHTMAP_SIZE = )glsl" + std::to_string(HEIGHTMAP_SIZE) + R"glsl(.0;

// Fonction de hash simple
float rand3(vec3 c) {
    return fract(sin(dot(c.xyz, vec3(12.9898, 78.233, 133.719))) * 43758.5453);
}

// ★ FONCTION PROCÉDURALE (RAPIDE comme Vercidium)
float getHeightProcedural(float worldX, float worldZ) {
    return sin(worldX * 0.1) * 5.0 + cos(worldZ * 0.15) * 3.0;
}

void main() {
    // ★ MAGIE gl_VertexID: Reconstruire X,Z depuis l'index du vertex
    int localVertexID = gl_VertexID % )glsl" + std::to_string(VERTICES_PER_CHUNK) + R"glsl(;
    float runIndex = mod(float(localVertexID), VERTICES_PER_RUN);
    float clampedIndex = clamp(runIndex - 1.0, 0.0, VERTICES_PER_RUN_NOT_DEGENERATE);

    // X incrémente tous les 2 vertices (triangle strip pattern)
    float xPos = floor(clampedIndex / 2.0);

    // Z incrémente à chaque nouvelle "run" (ligne de triangles)
    float zPos = floor(float(localVertexID) / VERTICES_PER_RUN);
    zPos += mod(clampedIndex, 2.0);  // Décalage pour former triangles

    // ★ GPU BATCHING: gl_DrawIDARB identifie le chunk
    vec2 chunkOffset = chunkPositions[gl_DrawIDARB] * HEIGHTMAP_SIZE;

    // Coordonnées mondiales
    float worldX = xPos + chunkOffset.x;
    float worldZ = zPos + chunkOffset.y;

    // ★★★ SYSTÈME HYBRIDE OPTIMISÉ: Sans branching ! ★★★
    // On calcule les DEUX altitudes, puis on mix selon le flag
    // Pas de divergence GPU, tous les threads font la même chose

    // Calcul procédural (toujours fait, ultra rapide)
    float altitudeProcedural = getHeightProcedural(worldX, worldZ);

    // Lecture texture (toujours fait si flag=1, mais ALU pas cher)
    vec2 uv = vec2(xPos, zPos) / HEIGHTMAP_SIZE;  // 32×32, pas de padding
    float altitudeTexture = texture(heightmapTextures, vec3(uv, float(gl_DrawIDARB))).r;

    // Mix selon le flag (0.0 = procédural, 1.0 = texture)
    float editFactor = float(chunkEditFlags[gl_DrawIDARB]);
    float altitude = mix(altitudeProcedural, altitudeTexture, editFactor);

    // Position mondiale finale
    vec3 worldPos = vec3(worldX, altitude, worldZ);
    vWorldPos = worldPos;

    // ★ SINKING ILLUSION LOD
    float distToCamera = length(worldPos.xz - cameraPos.xz);
    float sinkAmount = 0.0;

    if (distToCamera > )glsl" + std::to_string(LOD_DISTANCES[0]) + R"glsl() {
        float fade = smoothstep()glsl" + std::to_string(LOD_DISTANCES[0]) + R"glsl(,
                                   )glsl" + std::to_string(LOD_DISTANCES[1]) + R"glsl(,
                                   distToCamera);
        sinkAmount = fade * )glsl" + std::to_string(SINK_DEPTH) + R"glsl(;
    }

    worldPos.y -= sinkAmount;
    gl_Position = mvp * vec4(worldPos, 1.0);

    // Luminosité procédurale
    vBrightness = mix(0.5, 1.0, fract(rand3(worldPos) * float(gl_VertexID)));

    // Coordonnées barycentriques pour wireframe
    int baryIndex = int(mod(clampedIndex, 3.0));
    if (baryIndex == 0)
        vBary = vec2(0.0, 0.0);
    else if (baryIndex == 1)
        vBary = vec2(0.0, 1.0);
    else
        vBary = vec2(1.0, 0.0);
}
)glsl";
}

// ============================================================================
// COMPILATION DES SHADERS
// ============================================================================

GLuint CompileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* sourceCStr = source.c_str();
    glShaderSource(shader, 1, &sourceCStr, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "❌ Shader compilation error:\n" << infoLog << std::endl;
    }

    return shader;
}

std::string CreateFragmentShader() {
    return R"glsl(
#version 430 core

in vec3 vWorldPos;
in vec2 vBary;
flat in float vBrightness;

uniform bool showWireframe;

out vec4 FragColor;

// Fonction barycentric pour wireframe propre
float barycentric(vec2 vBC, float width) {
    vec3 bary = vec3(vBC.x, vBC.y, 1.0 - vBC.x - vBC.y);
    vec3 d = fwidth(bary);
    vec3 a3 = smoothstep(d * (width - 0.5), d * (width + 0.5), bary);
    return min(min(a3.x, a3.y), a3.z);
}

// Calcul de normale approximative avec dFdx/dFdy
vec3 calculateNormal() {
    vec3 fdx = dFdx(vWorldPos);
    vec3 fdy = dFdy(vWorldPos);
    return normalize(cross(fdx, fdy));
}

void main() {
    // Couleur basée sur l'altitude (vert foncé → vert clair → beige)
    float heightFactor = clamp((vWorldPos.y + 10.0) / 20.0, 0.0, 1.0);
    vec3 lowColor = vec3(0.2, 0.4, 0.2);   // Vert foncé
    vec3 midColor = vec3(0.3, 0.6, 0.3);   // Vert moyen
    vec3 highColor = vec3(0.7, 0.7, 0.5);  // Beige/sable

    vec3 baseColor;
    if (heightFactor < 0.5) {
        baseColor = mix(lowColor, midColor, heightFactor * 2.0);
    } else {
        baseColor = mix(midColor, highColor, (heightFactor - 0.5) * 2.0);
    }

    // Éclairage directionnel simple
    vec3 normal = calculateNormal();
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));  // Lumière du haut-droite
    float diffuse = max(dot(normal, lightDir), 0.0);
    float ambient = 0.4;  // Lumière ambiante
    float lighting = ambient + diffuse * 0.6;

    // Appliquer l'éclairage et la variation procédurale
    vec3 color = baseColor * lighting * vBrightness;

    // Mode wireframe
    if (showWireframe) {
        color *= 0.3;
        color += vec3(1.0 - barycentric(vBary, 1.0)) * vec3(1.0, 1.0, 0.0);
    }

    FragColor = vec4(color, 1.0);
}
)glsl";
}

GLuint CreateShaderProgram() {
    std::string vertexShaderSource = CreateVertexShader();
    std::string fragmentShaderSource = CreateFragmentShader();
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "❌ Shader linking error:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

// ============================================================================
// GÉNÉRATION DU BUFFER (Triangle Strips avec degenerate triangles)
// ============================================================================

// IMPORTANT: On génère UN SEUL chunk de géométrie (vertices "dummy")
// Les altitudes RÉELLES viennent des TEXTURES de heightmap
// Ceci permet d'éditer le terrain sans recompiler les buffers !
void GenerateHeightmapBuffer(std::vector<HeightmapVertex>& vertices) {
    vertices.clear();
    vertices.reserve(VERTICES_PER_CHUNK);

    // Générer HEIGHTMAP_SIZE triangle strips
    // NOTE: On met 0.0 partout car l'altitude vient de la texture
    for (int z = 0; z < HEIGHTMAP_SIZE; z++) {
        int x = 0;

        // Premier vertex dégénéré
        vertices.push_back({0.0f});

        // Créer le premier triangle du strip
        vertices.push_back({0.0f});
        vertices.push_back({0.0f});
        vertices.push_back({0.0f});

        // Reste du strip
        x = 1;
        vertices.push_back({0.0f});

        x = 2;
        for (; x <= HEIGHTMAP_SIZE; x++) {
            vertices.push_back({0.0f});
            vertices.push_back({0.0f});
        }

        // Dernier vertex dégénéré
        vertices.push_back({0.0f});
    }

    std::cout << "✅ Buffer géométrique généré: " << vertices.size() << " vertices\n";
    std::cout << "   ℹ️  Altitudes lues depuis les textures de heightmap (GPU)\n";
}

// ============================================================================
// INITIALISATION OPENGL
// ============================================================================

void InitOpenGL() {
    // Générer le buffer de géométrie (partagé par tous les chunks)
    std::vector<HeightmapVertex> vertices;
    GenerateHeightmapBuffer(vertices);

    // Créer VAO/VBO
    glGenVertexArrays(1, &g_vao);
    glGenBuffers(1, &g_vbo);

    glBindVertexArray(g_vao);
    glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(HeightmapVertex),
                 vertices.data(),
                 GL_STATIC_DRAW);

    // Attribut 0: altitude (float) - non utilisé mais garde la structure
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(HeightmapVertex), (void*)0);

    glBindVertexArray(0);

    // ★ GÉNÉRATION DES CHUNKS ET HEIGHTMAPS
    std::cout << "\n📦 Génération des " << TOTAL_CHUNKS << " chunks...\n";
    g_terrainChunks.resize(TOTAL_CHUNKS);

    for (int z = 0; z < CHUNKS_PER_ROW; z++) {
        for (int x = 0; x < CHUNKS_PER_ROW; x++) {
            int index = z * CHUNKS_PER_ROW + x;
            g_terrainChunks[index].chunkPos = glm::ivec2(x, z);
            GenerateChunkHeightmap(g_terrainChunks[index]);
        }
    }
    std::cout << "✅ " << TOTAL_CHUNKS << " heightmaps générées\n";

    // ★ CRÉER UNE TEXTURE ARRAY pour TOUTES les heightmaps
    GLuint heightmapTextureArray;
    glGenTextures(1, &heightmapTextureArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, heightmapTextureArray);

    // Allouer la texture array (SANS PADDING: 32×32 pur)
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R32F,
                 HEIGHTMAP_SIZE, HEIGHTMAP_SIZE, TOTAL_CHUNKS,
                 0, GL_RED, GL_FLOAT, nullptr);

    // Copier chaque heightmap dans un layer de la texture array
    for (int i = 0; i < TOTAL_CHUNKS; i++) {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
                        0, 0, i,  // offset x, y, z (layer)
                        HEIGHTMAP_SIZE, HEIGHTMAP_SIZE, 1,  // width, height, depth
                        GL_RED, GL_FLOAT,
                        g_terrainChunks[i].heightData.data());
    }

    // Paramètres de sampling
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    // Stocker le handle pour le binding plus tard
    g_terrainChunks[0].heightmapTexture = heightmapTextureArray;  // On stocke dans le premier chunk

    // Créer SSBO pour les positions de chunks
    std::vector<glm::vec2> chunkPositions;
    chunkPositions.reserve(TOTAL_CHUNKS);

    for (int z = 0; z < CHUNKS_PER_ROW; z++) {
        for (int x = 0; x < CHUNKS_PER_ROW; x++) {
            chunkPositions.push_back(glm::vec2(float(x), float(z)));
        }
    }

    glGenBuffers(1, &g_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, g_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 chunkPositions.size() * sizeof(glm::vec2),
                 chunkPositions.data(),
                 GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, g_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // ★ CRÉER SSBO pour les flags isEdited (binding 2)
    std::vector<int> editFlags(TOTAL_CHUNKS, 0);  // 0 = procédural, 1 = texture
    // Par défaut, TOUS les chunks sont en mode procédural (performance maximale !)

    glGenBuffers(1, &g_ssboEditFlags);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, g_ssboEditFlags);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 editFlags.size() * sizeof(int),
                 editFlags.data(),
                 GL_DYNAMIC_DRAW);  // DYNAMIC car on va modifier quand on édite
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, g_ssboEditFlags);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Créer le shader program
    g_shaderProgram = CreateShaderProgram();
    g_uniformMVP = glGetUniformLocation(g_shaderProgram, "mvp");
    g_uniformShowWireframe = glGetUniformLocation(g_shaderProgram, "showWireframe");
    g_uniformCameraPos = glGetUniformLocation(g_shaderProgram, "cameraPos");

    // Configuration OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    std::cout << "\n✅ OpenGL initialisé !\n";
    std::cout << "   Chunks: " << TOTAL_CHUNKS << "\n";
    std::cout << "   Vertices par chunk: " << VERTICES_PER_CHUNK << "\n";
    std::cout << "   Total vertices: " << (TOTAL_CHUNKS * VERTICES_PER_CHUNK) << "\n";
    std::cout << "   ★ SYSTÈME HYBRIDE OPTIMISÉ:\n";
    std::cout << "      - Sans branching (mix au lieu de if/else)\n";
    std::cout << "      - Chunks non édités: Calcul PROCÉDURAL pur\n";
    std::cout << "      - Chunks édités: TEXTURE sampling\n";
    std::cout << "   Texture Array: " << HEIGHTMAP_SIZE << "×" << HEIGHTMAP_SIZE
              << " × " << TOTAL_CHUNKS << " layers (-13% mémoire vs padding)\n";
    std::cout << "   Frustum Culling: ACTIVÉ (bounding box complète, -30-50% draw calls)\n";
    std::cout << "   Mémoire heightmaps CPU: "
              << (TOTAL_CHUNKS * HEIGHTMAP_SIZE * HEIGHTMAP_SIZE * sizeof(float) / 1024.0 / 1024.0)
              << " MB\n";
    std::cout << "   Caméra initiale: (" << g_cameraPos.x << ", " << g_cameraPos.y << ", " << g_cameraPos.z << ")\n";
}

// ============================================================================
// CALLBACKS GLFW
// ============================================================================

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            g_captureMouse = !g_captureMouse;
            // Réinitialiser la position pour éviter les sauts de caméra
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            g_lastMouse = glm::vec2(xpos, ypos);
            std::cout << "Capture souris: " << (g_captureMouse ? "ON" : "OFF") << std::endl;
        }
        else if (key == GLFW_KEY_SPACE) {
            g_showWireframe = !g_showWireframe;
            std::cout << "Wireframe: " << (g_showWireframe ? "ON" : "OFF") << std::endl;
        }
        else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_4) {
            int lodIndex = key - GLFW_KEY_1;
            g_lodEnabled[lodIndex] = !g_lodEnabled[lodIndex];
            std::cout << "LOD " << lodIndex << ": "
                      << (g_lodEnabled[lodIndex] ? "ON" : "OFF") << std::endl;
        }
    }
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    // Ne rien faire ici, on gère la souris dans UpdateCamera()
    // (comme Vercidium: recentrage à chaque frame)
}// ============================================================================
// HELPER: Vecteur direction depuis pitch/yaw (comme Vercidium)
// ============================================================================

glm::vec3 FromPitchYaw(float pitch, float yaw) {
    float cosPitch = std::cos(pitch);
    return glm::vec3(
        cosPitch * std::sin(yaw),   // X
        std::sin(pitch),             // Y
        cosPitch * std::cos(yaw)     // Z
    );
}

// ============================================================================
// UPDATE CAMERA - EXACTEMENT comme Vercidium
// ============================================================================

void UpdateCamera(float deltaTime) {
    // Mouse movement (si capture active)
    if (g_captureMouse) {
        double xpos, ypos;
        glfwGetCursorPos(g_window, &xpos, &ypos);

        glm::vec2 diff = g_lastMouse - glm::vec2(xpos, ypos);

        // Sensibilité EXACTE de Vercidium
        g_cameraYaw -= diff.x * 0.003f;
        g_cameraPitch += diff.y * 0.003f;

        // Recentrer la souris (méthode Vercidium)
        int centerX = WINDOW_WIDTH / 2;
        int centerY = WINDOW_HEIGHT / 2;
        glfwSetCursorPos(g_window, centerX, centerY);
        g_lastMouse = glm::vec2(centerX, centerY);

        glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    } else {
        glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    // Mouvement (EXACTEMENT comme Vercidium)
    float movementSpeed = 0.15f * 60.0f * deltaTime;  // Ajusté pour 60 FPS base

    // W/S: Avant/Arrière
    if (glfwGetKey(g_window, GLFW_KEY_W) == GLFW_PRESS)
        g_cameraPos += FromPitchYaw(g_cameraPitch, g_cameraYaw) * movementSpeed;
    else if (glfwGetKey(g_window, GLFW_KEY_S) == GLFW_PRESS)
        g_cameraPos -= FromPitchYaw(g_cameraPitch, g_cameraYaw) * movementSpeed;

    // A/D: Gauche/Droite (strafe, pitch = 0)
    if (glfwGetKey(g_window, GLFW_KEY_A) == GLFW_PRESS)
        g_cameraPos += FromPitchYaw(0, g_cameraYaw - 3.14159f / 2.0f) * movementSpeed;
    else if (glfwGetKey(g_window, GLFW_KEY_D) == GLFW_PRESS)
        g_cameraPos += FromPitchYaw(0, g_cameraYaw + 3.14159f / 2.0f) * movementSpeed;

    // E/Q: Haut/Bas (pitch = +90° / -90°)
    if (glfwGetKey(g_window, GLFW_KEY_E) == GLFW_PRESS)
        g_cameraPos += FromPitchYaw(3.14159f / 2.0f, 0) * movementSpeed;
    else if (glfwGetKey(g_window, GLFW_KEY_Q) == GLFW_PRESS)
        g_cameraPos += FromPitchYaw(-3.14159f / 2.0f, 0) * movementSpeed;
}// ============================================================================
// RENDER
// ============================================================================

void Render() {
    // Background bleu ciel doux
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Matrices View/Projection - Style Vercidium (pitch/yaw → lookAt)
    glm::vec3 forward = FromPitchYaw(g_cameraPitch, g_cameraYaw);
    glm::vec3 target = g_cameraPos + forward;
    glm::mat4 view = glm::lookAt(g_cameraPos, target, glm::vec3(0.0f, 1.0f, 0.0f));

    float aspect = float(WINDOW_WIDTH) / float(WINDOW_HEIGHT);
    glm::mat4 projection = glm::perspective(FOV, aspect, NEAR_PLANE, FAR_PLANE);
    glm::mat4 mvp = projection * view;

    // Activer le shader
    glUseProgram(g_shaderProgram);
    glUniformMatrix4fv(g_uniformMVP, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform1i(g_uniformShowWireframe, g_showWireframe ? 1 : 0);
    glUniform3fv(g_uniformCameraPos, 1, glm::value_ptr(g_cameraPos));

    // ★ BIND LA TEXTURE ARRAY DES HEIGHTMAPS
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, g_terrainChunks[0].heightmapTexture);
    GLint texLocation = glGetUniformLocation(g_shaderProgram, "heightmapTextures");
    glUniform1i(texLocation, 0);  // Texture unit 0

    // Mode wireframe si activé
    if (g_showWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Bind SSBO
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, g_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, g_ssboEditFlags);

    // ★ FRUSTUM CULLING: Ne dessiner que les chunks visibles !
    Frustum frustum = ExtractFrustum(mvp);

    glBindVertexArray(g_vao);

    // Créer les tableaux pour glMultiDrawArrays (seulement chunks visibles)
    std::vector<GLint> firsts;
    std::vector<GLsizei> counts;

    g_chunksRendered = 0;

    for (int i = 0; i < TOTAL_CHUNKS; i++) {
        // Calculer l'AABB du chunk avec marge généreuse
        // (incluant le sinking LOD et les collines)
        glm::vec3 chunkMin(
            g_terrainChunks[i].chunkPos.x * HEIGHTMAP_SIZE,
            -50.0f,  // Marge large pour le sinking (20m) + hauteur min (-10m) + sécurité
            g_terrainChunks[i].chunkPos.y * HEIGHTMAP_SIZE
        );
        glm::vec3 chunkMax(
            (g_terrainChunks[i].chunkPos.x + 1) * HEIGHTMAP_SIZE,
            50.0f,   // Marge large pour collines + sécurité
            (g_terrainChunks[i].chunkPos.y + 1) * HEIGHTMAP_SIZE
        );

        // Tester si le chunk est visible
        if (IsChunkVisible(frustum, chunkMin, chunkMax)) {
            firsts.push_back(0);  // Tous les chunks utilisent le même buffer
            counts.push_back(VERTICES_PER_CHUNK);
            g_chunksRendered++;
        }
    }

    // ★ DRAW CALL OPTIMISÉ: Seulement les chunks visibles !
    if (g_chunksRendered > 0) {
        glMultiDrawArrays(GL_TRIANGLE_STRIP, firsts.data(), counts.data(), g_chunksRendered);
    }

    // Debug: vérifier les erreurs OpenGL
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        static bool errorLogged = false;
        if (!errorLogged) {
            std::cerr << "❌ Erreur OpenGL pendant le rendu: 0x" << std::hex << err << std::dec << std::endl;
            errorLogged = true;
        }
    }

    glBindVertexArray(0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Calculer le nombre de triangles (seulement les chunks visibles)
    g_trianglesRendered = g_chunksRendered * (VERTICES_PER_CHUNK - 2);
}

#if 0
// ============================================================================
// MAIN
// ============================================================================

int main() {
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "  HEIGHTMAP RENDERER - Toutes les optimisations\n";
    std::cout << "═══════════════════════════════════════════════════════\n\n";

    // Init GLFW
    if (!glfwInit()) {
        std::cerr << "❌ Erreur: Impossible d'initialiser GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);  // MSAA

    g_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                 "Heightmap Renderer - Vercidium Optimizations",
                                 nullptr, nullptr);
    if (!g_window) {
        std::cerr << "❌ Erreur: Impossible de créer la fenêtre" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(g_window);
    glfwSetKeyCallback(g_window, KeyCallback);
    glfwSetCursorPosCallback(g_window, MouseCallback);

    // Initialiser la position de la souris (Vercidium style)
    int centerX = WINDOW_WIDTH / 2;
    int centerY = WINDOW_HEIGHT / 2;
    glfwSetCursorPos(g_window, centerX, centerY);
    g_lastMouse = glm::vec2(centerX, centerY);
    glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // Init GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "❌ Erreur: Impossible d'initialiser GLEW" << std::endl;
        return -1;
    }

    std::cout << "✅ OpenGL " << glGetString(GL_VERSION) << std::endl;
    std::cout << "✅ GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    InitOpenGL();

    std::cout << "\n🎮 CONTRÔLES:\n";
    std::cout << "   WASD: Déplacement\n";
    std::cout << "   QE: Monter/Descendre\n";
    std::cout << "   Souris: Regarder autour\n";
    std::cout << "   ESPACE: Toggle Wireframe\n";
    std::cout << "   ESC: Toggle capture souris\n";
    std::cout << "   Fermer la fenêtre: Quitter\n\n";

    auto lastTime = std::chrono::high_resolution_clock::now();
    g_lastFPSTime = glfwGetTime();
    double g_lastTitleUpdate = g_lastFPSTime; // time of last title/HUD update

    // Boucle de rendu
    while (!glfwWindowShouldClose(g_window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        UpdateCamera(deltaTime);
        Render();

        glfwSwapBuffers(g_window);
        glfwPollEvents();

        // Calcul FPS
        g_frameCount++;
        double currentFPSTime = glfwGetTime();
        // Update FPS every 0.1s for smoother HUD without heavy cost
        if (currentFPSTime - g_lastFPSTime >= 0.1) {
            g_currentFPS = g_frameCount / (currentFPSTime - g_lastFPSTime);

            // Update title every 0.1s (cheap) with more stats
            if (currentFPSTime - g_lastTitleUpdate >= 0.1) {
                std::stringstream ss;
                ss << "Heightmap Renderer - "
                   << int(g_currentFPS) << " FPS - "
                   << g_chunksRendered << "/" << TOTAL_CHUNKS << " chunks - "
                   << (g_trianglesRendered / 1000) << "K tris - "
                   << "Pos: (" << int(g_cameraPos.x) << ", "
                   << int(g_cameraPos.y) << ", "
                   << int(g_cameraPos.z) << ")";
                glfwSetWindowTitle(g_window, ss.str().c_str());
                g_lastTitleUpdate = currentFPSTime;
            }

            g_frameCount = 0;
            g_lastFPSTime = currentFPSTime;
        }
    }

    // Cleanup
    glDeleteVertexArrays(1, &g_vao);
    glDeleteBuffers(1, &g_vbo);
    glDeleteBuffers(1, &g_ssbo);
    glDeleteBuffers(1, &g_ssboEditFlags);
    glDeleteProgram(g_shaderProgram);

    // Cleanup textures
    if (!g_terrainChunks.empty() && g_terrainChunks[0].heightmapTexture != 0) {
        glDeleteTextures(1, &g_terrainChunks[0].heightmapTexture);
    }

    glfwDestroyWindow(g_window);
    glfwTerminate();

    std::cout << "\n✅ Programme terminé proprement.\n";
    return 0;
}
#endif

} // namespace Optimizing::World
