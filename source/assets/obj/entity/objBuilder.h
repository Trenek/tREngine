#ifndef OBJ_BUILDER
#define OBJ_BUILDER
#include <vulkan/vulkan.h>

#include "instanceBuilder.h"

struct Model;
struct GraphicsSetup;

struct ObjPushConstants {
    int textureOffset;
};

struct ObjBuilder {
    uint32_t instanceCount;

    struct instanceBuilder instance;
    struct Model *modelData;

    VkDescriptorSetLayout objectLayout;
    size_t textureOffset;
};

struct Entity *createObj(struct ObjBuilder builder, struct GraphicsSetup *graphics);

#endif
