#include <vulkan/vulkan.h>
#include <stdlib.h>
#include <stdio.h>

#include "MY_ASSERT.h"

#include "graphicsSetup.h"

#include "definitions.h"
#include "descriptor.h"

#include "descriptorObj.h"

struct DescriptorObj *createDescriptorSetsObj(struct GraphicsSetup *graphics, struct DescriptorObjBuilder *builder) {
    struct DescriptorObj *descriptorObj = calloc(1, sizeof(struct DescriptorObj));

    descriptorObj->device = graphics->device;
    MY_ASSERT(VK_SUCCESS == vkCreateDescriptorPool(graphics->device, &(VkDescriptorPoolCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = builder->qDescriptorPoolSize,
        .pPoolSizes = builder->descriptorPoolSize,
        .maxSets = MAX_FRAMES_IN_FLIGHT,
    }, nullptr, &descriptorObj->descriptorPool));
    
    createDescriptorSets(descriptorObj->descriptorSets, graphics->device, descriptorObj->descriptorPool, builder->layout);

    return descriptorObj;
}

void destroyDescriptorSets(void *thisPtr) {
    struct DescriptorObj *this = thisPtr;

    vkDestroyDescriptorPool(this->device, this->descriptorPool, NULL);

    free(this);
}
