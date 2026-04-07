#include "Vertex.h"

#define OBJ(X) ((struct ObjVertex *)X)

struct ObjVertex {
    vec3 geo;
    vec2 tex;
    vec3 norm;
    vec2 para;
    unsigned int material;
};

[[maybe_unused]]
static VkVertexInputAttributeDescription ObjVertexAttributeDescriptions[] = {
    [0] = {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(struct ObjVertex, geo)
    },
    [1] = {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(struct ObjVertex, tex)
    },
    [2] = {
        .binding = 0,
        .location = 2,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(struct ObjVertex, norm)
    },
    [3] = {
        .binding = 0,
        .location = 3,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(struct ObjVertex, para)
    },
    [4] = {
        .binding = 0,
        .location = 4,
        .format = VK_FORMAT_R32_UINT,
        .offset = offsetof(struct ObjVertex, material)
    },
};

