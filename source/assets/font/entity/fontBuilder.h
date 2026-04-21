#ifndef STRING_BUILDER
#define STRING_BUILDER
#include <vulkan/vulkan.h>

struct Model;
struct GraphicsSetup;

struct FontPushConstants {
    int meshID;
};

struct FontBuilder {
    uint32_t instanceCount;

    size_t instanceSize;
    size_t instanceBufferSize;
    void (*instanceUpdater)(void *instancePtr, void *instanceBufferPtr, uint32_t instanceCount, float deltaTime);

    const char *string;
    struct Model *modelData;

    VkDescriptorSetLayout objectLayout;
    int center;
};

struct Entity *createFont(struct FontBuilder builder, struct GraphicsSetup *graphics);

#endif
