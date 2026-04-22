#include "Vertex.h"
#include "buffer.h"

#define FNT(x) ((struct FontVertex *)(x))

struct FontModelInfo {
    VkDevice device;

    struct buffer *buffers;

    void *pushConstants;
};

struct FontVertex {
    vec2 pos;
    vec3 color;
    vec2 bezzier;
    uint32_t inOut;
};

struct descriptorSetLayout *defaultFontDescriptorSetLayout(VkDevice device);

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

