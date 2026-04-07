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

#endif
