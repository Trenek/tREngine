#ifndef REC_BUILDER
#define REC_BUILDER
#include <vulkan/vulkan.h>

#include "definitions.h"

struct Model;
struct GraphicsSetup;

struct RecBuilder {
    uint32_t instanceCount;

    size_t instanceSize;
    size_t instanceBufferSize;
    void (*instanceUpdater)(void *instancePtr, void *instanceBufferPtr, uint32_t instanceCount, float deltaTime);

    struct Model *modelData;

    VkDescriptorSetLayout objectLayout;
    size_t textureOffset;
};

struct Entity *createRec(struct RecBuilder builder, struct GraphicsSetup *graphics);

#endif
