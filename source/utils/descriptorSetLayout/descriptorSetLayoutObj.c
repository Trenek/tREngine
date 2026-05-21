#include <vulkan/vulkan.h>

#include <stdlib.h>
#include <stdio.h>

#include "MY_ASSERT.h"

#include "descriptorSetLayoutObj.h"

struct DescriptorSetLayout *createDescriptorSetLayoutObj(size_t qBinding, VkDescriptorSetLayoutBinding binding[qBinding], VkDevice device) {
    struct DescriptorSetLayout *result = calloc(1, sizeof(struct DescriptorSetLayout));

    *result = (struct DescriptorSetLayout) {
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
    struct DescriptorSetLayout *layout = layoutPtr;

    vkDestroyDescriptorSetLayout(layout->device, layout->descriptorSetLayout, NULL);
    free(layout);
}
