#ifndef DESCRIPTOR_SET_LAYOUT_OBJ_H
#define DESCRIPTOR_SET_LAYOUT_OBJ_H

#include <vulkan/vulkan_core.h>

struct DescriptorSetLayout {
    VkDevice device;
    VkDescriptorSetLayout descriptorSetLayout;
};

struct DescriptorSetLayout *createDescriptorSetLayoutObj(size_t qBinding, VkDescriptorSetLayoutBinding binding[qBinding], VkDevice device);
void destroyDescriptorSetLayout(void *layoutPtr);

#endif
