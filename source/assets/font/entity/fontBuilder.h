#ifndef STRING_BUILDER
#define STRING_BUILDER
#include <vulkan/vulkan.h>

#include "instanceBuilder.h"

struct Model;
struct GraphicsSetup;

struct FontPushConstants {
    int meshID;
};

struct FontBuilder {
    uint32_t instanceCount;

    struct instanceBuilder instance;
    const char *string;
    struct Model *modelData;

    VkDescriptorSetLayout objectLayout;
    int center;
};

struct Entity *createFont(struct FontBuilder builder, struct GraphicsSetup *graphics);

#endif
