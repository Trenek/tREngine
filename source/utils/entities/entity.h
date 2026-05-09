#ifndef ENTITY_H
#define ENTITY_H

#include <vulkan/vulkan.h>

#include "descriptor.h"
#include "buffer.h"
#include "entityBuilder.h"
#include "instanceBuilder.h"
#include "definitions.h"

struct DrawCall2 {
    VkBuffer *vertexBuffer;

    size_t verticesQuantity;
};

struct DrawCall {
    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;

    union {
        size_t indicesQuantity;
        size_t verticesQuantity;
    };

    size_t pushConstantsSize;
    VkShaderStageFlags pushConstantsStage;
    void *pushConstans;

    size_t instanceCount;
};

struct Entity {
    VkDevice device;
    uint32_t instanceCount;
    void *instance;
    void (*instanceUpdater)(void *instancePtr, void *instanceBufferPtr, uint32_t instanceCount, float deltaTime);

    size_t qBuff;
    void **buffer;
    void *(**mapp)[MAX_FRAMES_IN_FLIGHT];
    size_t *range;

    uint32_t drawCallQuantity;
    void *drawCall;

    struct descriptor object;

    void *additional;
    void (*cleanup)(void *);
};

struct GraphicsSetup;
struct EntityBuilder;

struct Entity *createEntity(struct EntityBuilder modelBuilder, struct GraphicsSetup *graphics);
struct Entity *createInstancedEntity(struct EntityBuilder builder, struct GraphicsSetup *graphics);
void destroyEntity(void *modelPtr);
void destroyEntityArray(size_t num, struct Entity *modelArray[num], struct GraphicsSetup *graphics);

#endif
