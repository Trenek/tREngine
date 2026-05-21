#include <vulkan/vulkan.h>
#include <stdlib.h>
#include <stdio.h>

#include "MY_ASSERT.h"

#include "graphicsSetup.h"

#include "definitions.h"

#include "descriptorObj.h"

static void fillArray(int num, VkDescriptorSetLayout layouts[], VkDescriptorSetLayout elem) {
    while (num --> 0) {
        layouts[num] = elem;
    }
}

static void createDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout) {
    VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT]; {
        fillArray(MAX_FRAMES_IN_FLIGHT, layouts, descriptorSetLayout);
    };
    VkDescriptorSetAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = MAX_FRAMES_IN_FLIGHT,
        .pSetLayouts = layouts
    };

    MY_ASSERT(VK_SUCCESS == vkAllocateDescriptorSets(device, &allocInfo, descriptorSets));
}

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

void bindBuffersToDescriptorSets(struct DescriptorObj *descriptor, VkDevice device, size_t qBuff, VkBuffer buff[qBuff], size_t range[qBuff], bool isSingle[qBuff], VkDescriptorType descriptorType) {
    VkWriteDescriptorSet descriptorWrites[qBuff];
    VkDescriptorBufferInfo bufferInfo[qBuff];

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        for (size_t j = 0; j < qBuff; j += 1) {
            bufferInfo[j] = (VkDescriptorBufferInfo) {
                .buffer = buff[j],
                .offset = !isSingle[j] * i * range[j],
                .range = range[j]
            };
            descriptorWrites[j] = (VkWriteDescriptorSet){
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = descriptor->descriptorSets[i],
                .dstBinding = j,
                .dstArrayElement = 0,
                .descriptorType = descriptorType,
                .descriptorCount = 1,
                .pBufferInfo = &bufferInfo[j],
                .pTexelBufferView = NULL
            };
        }

        vkUpdateDescriptorSets(device, qBuff, descriptorWrites, 0, NULL);
    }
}
