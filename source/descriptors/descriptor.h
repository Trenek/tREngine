#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include <vulkan/vulkan_core.h>

#include "definitions.h"

struct descriptor {
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSets[MAX_FRAMES_IN_FLIGHT];
};

VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device, size_t qBindings, VkDescriptorSetLayoutBinding bindings[qBindings]);
VkDescriptorPool createDescriptorPool(VkDevice device, size_t qBuff, VkDescriptorType descriptorType);
void bindBuffersToDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, size_t qBuff, VkBuffer buff[qBuff], size_t range[qBuff], bool isSingle[qBuff], VkDescriptorType descriptorType);

void createDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout);

#endif
