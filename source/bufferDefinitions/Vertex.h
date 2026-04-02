#ifndef STRUCT_VERTEX
#define STRUCT_VERTEX

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <cglm/cglm.h>
#include <vulkan/vulkan_core.h>

#define Vert(x) \
    .sizeOfVertex = sizeof(struct x), \
    .numOfAttributes = sizeof(x##AttributeDescriptions) / sizeof(VkVertexInputAttributeDescription), \
    .attributeDescription = x##AttributeDescriptions

struct ObjVertex {
    vec3 geo;
    vec3 tex;
    vec3 norm;
    vec2 para;
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
        .format = VK_FORMAT_R32G32B32_SFLOAT,
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
};

struct FontVertex {
    vec2 pos;
    vec3 color;
    vec2 bezzier;
    uint32_t inOut;
};

[[maybe_unused]]
static VkVertexInputAttributeDescription FontVertexAttributeDescriptions[] = {
    [0] = {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(struct FontVertex, pos)
    },
    [1] = {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(struct FontVertex, color)
    },
    [2] = {
        .binding = 0,
        .location = 2,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(struct FontVertex, bezzier)
    },
    [3] = {
        .binding = 0,
        .location = 3,
        .format = VK_FORMAT_R32_UINT,
        .offset = offsetof(struct FontVertex, inOut)
    }
};

#endif
