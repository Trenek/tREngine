#ifndef STRUCT_VERTEX
#define STRUCT_VERTEX

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <cglm/cglm.h>
#include <stddef.h>
typedef struct VkVertexInputAttributeDescription VkVertexInputAttributeDescription;

struct Vert {
    size_t sizeOfVertex;
    size_t numOfAttributes;
    const VkVertexInputAttributeDescription *attributeDescription;
};

#endif
