#include "Vertex.h"
#include "buffer.h"

#define REC(X) ((struct RecVertex *)X)

struct RecVertex {
    vec3 pos;
    vec2 tex;
};

struct descriptorSetLayout *defaultRecDescriptorSetLayout(VkDevice device);
static inline struct Vert defaultRectVert() {
    static const VkVertexInputAttributeDescription vertexAttributeDescriptions[] = {
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

    return (struct Vert) {
        .sizeOfVertex = sizeof(struct RecVertex),
        .numOfAttributes = sizeof(vertexAttributeDescriptions) / sizeof(VkVertexInputAttributeDescription),
        .attributeDescription = vertexAttributeDescriptions
    };
}
