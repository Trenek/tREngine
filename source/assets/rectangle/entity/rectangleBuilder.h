#ifndef REC_BUILDER
#define REC_BUILDER
#include <vulkan/vulkan.h>

#include "instanceBuilder.h"

struct Model;
struct GraphicsSetup;

struct RecBuilder {
    uint32_t instanceCount;

    struct instanceBuilder instance;
    struct Model *modelData;

    VkDescriptorSetLayout objectLayout;
    size_t textureOffset;
};

struct Entity *createRec(struct RecBuilder builder, struct GraphicsSetup *graphics);
struct Entity *createInstancedRec(struct RecBuilder builder, struct GraphicsSetup *graphics);

#endif
