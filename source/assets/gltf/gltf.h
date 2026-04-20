#include "Vertex.h"
#include "buffer.h"

#define GLTF(X) ((struct GltfVertex *)X)
#define GLTF_PC(X) ((struct GltfPushConstants *)X)

#define ANIM_PATH_TYPE_MAX_ENUM 5

struct Frames {
    size_t qFrames;
    size_t qComponents;

    size_t interpolationType;

    float *values;
};

struct AnimationData {
    mat4 animation;
    float weight1;
    float weight2;
    float weight3;
    float weight4;
};

struct Entity;
struct Model;
void animate(struct Entity *, struct Model *, size_t, float);

struct GltfModelInfo {
    VkDevice device;

    size_t qBuffer;
    struct buffer *buffers;

    size_t qAnim;
    size_t qNodes;
    struct Frames (*frames)[ANIM_PATH_TYPE_MAX_ENUM];

    void *pushConstants;
};

struct GltfPushConstants {
    size_t nodeID;
};

struct GltfVertex {
    vec3 pos;
    vec3 norm;
    vec2 tex;
    vec3 color;

    vec3 morphPos1;
    vec3 morphPos2;
    vec3 morphPos3;
    vec3 morphPos4;
};

[[maybe_unused]]
static VkVertexInputAttributeDescription GltfVertexAttributeDescriptions[] = {
    [0] = {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(struct GltfVertex, pos)
    },
    [1] = {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(struct GltfVertex, norm)
    },
    [2] = {
        .binding = 0,
        .location = 2,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(struct GltfVertex, tex)
    },
    [3] = {
        .binding = 0,
        .location = 3,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(struct GltfVertex, color)
    },
    [4] = {
        .binding = 0,
        .location = 4,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(struct GltfVertex, morphPos1)
    },
    [5] = {
        .binding = 0,
        .location = 5,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(struct GltfVertex, morphPos2)
    },
    [6] = {
        .binding = 0,
        .location = 6,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(struct GltfVertex, morphPos3)
    },
    [7] = {
        .binding = 0,
        .location = 7,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(struct GltfVertex, morphPos4)
    },
};
