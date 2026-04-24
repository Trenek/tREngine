#ifndef OBJ_BUILDER
#define OBJ_BUILDER
#include <vulkan/vulkan.h>

struct Model;
struct GraphicsSetup;

struct ObjPushConstants {
    int textureOffset;
};

struct ObjBuilder {
    uint32_t instanceCount;

    size_t instanceSize;
    size_t instanceBufferSize;
    void (*instanceUpdater)(void *instancePtr, void *instanceBufferPtr, uint32_t instanceCount, float deltaTime);

    struct Model *modelData;

    VkDescriptorSetLayout objectLayout;
    size_t textureOffset;
};

struct Entity *createObj(struct ObjBuilder builder, struct GraphicsSetup *graphics);

#endif
