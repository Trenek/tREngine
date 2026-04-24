#ifndef GLTF_BUILDER
#define GLTF_BUILDER
#include <vulkan/vulkan.h>

struct Model;
struct GraphicsSetup;

struct GltfBuilder {
    uint32_t instanceCount;

    size_t instanceSize;
    size_t instanceBufferSize;
    void (*instanceUpdater)(void *instancePtr, void *instanceBufferPtr, uint32_t instanceCount, float deltaTime);

    struct Model *modelData;

    VkDescriptorSetLayout objectLayout;
    size_t textureOffset;
};

struct Entity *createGltf(struct GltfBuilder builder, struct GraphicsSetup *graphics);

#endif
