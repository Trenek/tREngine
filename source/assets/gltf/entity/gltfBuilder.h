#ifndef GLTF_BUILDER
#define GLTF_BUILDER
#include <vulkan/vulkan.h>

#include "instanceBuilder.h"

struct Model;
struct GraphicsSetup;

struct GltfBuilder {
    uint32_t instanceCount;

    struct instanceBuilder instance;
    struct Model *modelData;

    VkDescriptorSetLayout objectLayout;
    size_t textureOffset;
};

struct Entity *createGltf(struct GltfBuilder builder, struct GraphicsSetup *graphics);

#endif
