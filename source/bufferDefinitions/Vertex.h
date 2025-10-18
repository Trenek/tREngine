#ifndef STRUCT_VERTEX
#define STRUCT_VERTEX

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <cglm.h>
#include <vulkan/vulkan_core.h>

#define Vert(x) \
    .sizeOfVertex = sizeof(struct x), \
    .numOfAttributes = sizeof(x##AttributeDescriptions) / sizeof(VkVertexInputAttributeDescription), \
    .attributeDescription = x##AttributeDescriptions

struct GLTFVertex {
    vec3 pos;
    vec3 norm;
    vec3 color;
    vec2 texCoord;

    ivec4 bone;
    vec4 weights;
};

[[maybe_unused]]
static VkVertexInputAttributeDescription GLTFVertexAttributeDescriptions[] = {
    [0] = {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(struct GLTFVertex, pos)
    },
    [1] = {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(struct GLTFVertex, norm)
    },
    [2] = {
        .binding = 0,
        .location = 2,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(struct GLTFVertex, color)
    },
    [3] = {
        .binding = 0,
        .location = 3,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(struct GLTFVertex, texCoord)
    },
    [4] = {
        .binding = 0,
        .location = 4,
        .format = VK_FORMAT_R32G32B32A32_SINT,
        .offset = offsetof(struct GLTFVertex, bone)
    },
    [5] = {
        .binding = 0,
        .location = 5,
        .format = VK_FORMAT_R32G32B32A32_SFLOAT,
        .offset = offsetof(struct GLTFVertex, weights)
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
