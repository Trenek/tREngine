#include <vulkan/vulkan.h>
#include "Vertex.h"

#define REC2(X) ((struct Rec2Vertex *)X)

struct Rec2Vertex {
    vec3 pos;
    vec2 tex;
    vec4 color;
};

struct DescriptorSetLayout *defaultRec2DescriptorSetLayout(VkDevice device);
static inline struct Vert defaultRect2Vert() {
    static const VkVertexInputAttributeDescription vertexAttributeDescriptions[] = {
        [0] = {
            .binding = 0,
            .location = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(struct Rec2Vertex, pos)
        },
        [1] = {
            .binding = 0,
            .location = 1,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(struct Rec2Vertex, tex)
        },
        [2] = {
            .binding = 0,
            .location = 2,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(struct Rec2Vertex, color)
        },
    };

    return (struct Vert) {
        .sizeOfVertex = sizeof(struct Rec2Vertex),
        .numOfAttributes = sizeof(vertexAttributeDescriptions) / sizeof(VkVertexInputAttributeDescription),
        .attributeDescription = vertexAttributeDescriptions
    };
}
