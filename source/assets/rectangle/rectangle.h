#include "Vertex.h"
#include "buffer.h"

#define REC(X) ((struct RecVertex *)X)

struct RecVertex {
    vec3 pos;
    vec2 tex;
};

struct descriptorSetLayout *defaultRecDescriptorSetLayout(VkDevice device);

[[maybe_unused]]
static VkVertexInputAttributeDescription RecVertexAttributeDescriptions[] = {
    [0] = {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(struct RecVertex, pos)
    },
    [1] = {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(struct RecVertex, tex)
    },
};
