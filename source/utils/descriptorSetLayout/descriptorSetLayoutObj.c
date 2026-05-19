#include <vulkan/vulkan.h>

#include <stdlib.h>
#include <stdio.h>

#include "MY_ASSERT.h"

#include "descriptorSetLayoutObj.h"

struct descriptorSetLayout *createDescriptorSetLayoutObj(size_t qBinding, VkDescriptorSetLayoutBinding binding[qBinding], VkDevice device) {
    struct descriptorSetLayout *result = calloc(1, sizeof(struct descriptorSetLayout));

    *result = (struct descriptorSetLayout) {
        .device = device
    };

    MY_ASSERT(VK_SUCCESS == vkCreateDescriptorSetLayout(device, &(VkDescriptorSetLayoutCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = qBinding,
        .pBindings = binding,
    }, NULL, &result->descriptorSetLayout));

    return result;
}

void destroyDescriptorSetLayout(void *layoutPtr) {
    struct descriptorSetLayout *layout = layoutPtr;

    vkDestroyDescriptorSetLayout(layout->device, layout->descriptorSetLayout, NULL);
    free(layout);
}
