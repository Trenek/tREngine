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
void bindBuffersToDescriptorSets(struct DescriptorObj *descriptor, VkDevice device, size_t qBuff, VkBuffer buff[qBuff], size_t range[qBuff], bool isSingle[qBuff], VkDescriptorType descriptorType);
