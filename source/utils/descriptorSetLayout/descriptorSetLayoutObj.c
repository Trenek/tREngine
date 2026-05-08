#include <vulkan/vulkan.h>

#include <stdlib.h>
#include <stdio.h>

#include "descriptor.h"
#include "descriptorSetLayoutObj.h"

struct descriptorSetLayout *createDescriptorSetLayoutObj(size_t qBinding, VkDescriptorSetLayoutBinding binding[qBinding], VkDevice device) {
    struct descriptorSetLayout *result = calloc(1, sizeof(struct descriptorSetLayout));

    *result = (struct descriptorSetLayout) {
        .descriptorSetLayout = createDescriptorSetLayout(device, qBinding, binding),
        .device = device
    };

    return result;
}

void destroyDescriptorSetLayout(void *layoutPtr) {
    struct descriptorSetLayout *layout = layoutPtr;

    vkDestroyDescriptorSetLayout(layout->device, layout->descriptorSetLayout, NULL);
    free(layout);
}
