#ifndef ENTITY_BUILDER
#define ENTITY_BUILDER
#include <vulkan/vulkan.h>

#include "definitions.h"
#include "instanceBuilder.h"

struct Model;

struct EntityBuilder {
    uint32_t instanceCount;

    uint32_t meshQuantity;
    struct Mesh *mesh;

    int destination;
    size_t pushConstantsSize;
    void *pushConstants;

    VkBuffer (**buff);
    void *(**mapp)[MAX_FRAMES_IN_FLIGHT];
    bool *isChangable;
    size_t *range;
    size_t qBuff;

    VkDescriptorSetLayout objectLayout;

    struct instanceBuilder instance;

    void *additional;
    void (*cleanup)(void *);
};

#endif
