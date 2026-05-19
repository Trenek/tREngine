#include <vulkan/vulkan.h>

#include "graphicsSetup.h"

#include "definitions.h"

struct DescriptorObjBuilder {
    VkDescriptorSetLayout layout;

    size_t qDescriptorPoolSize;
    VkDescriptorPoolSize *descriptorPoolSize;
};

struct DescriptorObj {
    VkDevice device;

    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSets[MAX_FRAMES_IN_FLIGHT];
};

struct DescriptorObj *createDescriptorSetsObj(struct GraphicsSetup *graphics, struct DescriptorObjBuilder *builder);
void destroyDescriptorSets(void *thisPtr);
