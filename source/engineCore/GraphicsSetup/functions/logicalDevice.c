#include <vulkan/vulkan.h>

#include "MY_ASSERT.h"

#include "queueFamilyIndices.h"

extern const char *const deviceExtensions[];
extern const uint32_t deviceExtensionsCount;

static size_t removeDuplications(size_t queueNumber, VkDeviceQueueCreateInfo a[]) {
    size_t newSize = 0;

    bool found = false;

    for (size_t i = 0; i < queueNumber; i += 1) {
        found = false;

        for (size_t j = i + 1; found == false && j < queueNumber; j += 1) {
            found = a[i].queueFamilyIndex == a[j].queueFamilyIndex;
        }

        if (found == false) {
            a[newSize] = a[i];

            newSize += 1;
        }
    }

    return newSize;
}

VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, struct QueueFamilyIndices indices) {
    VkDevice device = NULL;

    VkDeviceQueueCreateInfo queueCreateInfo[] = {
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = indices.family[GRAPHICS_FAMILY].value,
            .queueCount = 1,
            .pQueuePriorities = (float[]) { 1.0f },
            .flags = 0,
            .pNext = NULL
        },
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = indices.family[PRESENT_FAMILY].value,
            .queueCount = 1,
            .pQueuePriorities = (float[]) { 1.0f },
            .flags = 0,
            .pNext = NULL
        },
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = indices.family[TRANSFER_FAMILY].value,
            .queueCount = 1,
            .pQueuePriorities = (float[]) { 1.0f },
            .flags = 0,
            .pNext = NULL
        },
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = indices.family[COMPUTE_FAMILY].value,
            .queueCount = 1,
            .pQueuePriorities = (float[]) { 1.0f },
            .flags = 0,
            .pNext = NULL
        }
    };
    size_t queueNumber = removeDuplications(sizeof(queueCreateInfo) / sizeof(VkDeviceQueueCreateInfo), queueCreateInfo);

    VkPhysicalDeviceDescriptorIndexingFeatures indexingFeature = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
        .pNext = NULL
    };

    VkPhysicalDeviceFeatures2 deviceFeatures = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &indexingFeature,
        .features = {
            .samplerAnisotropy = VK_TRUE,
            .depthBounds = VK_TRUE
        },
    };

    vkGetPhysicalDeviceFeatures2(physicalDevice, &deviceFeatures);

    VkPhysicalDeviceVulkan13Features features13 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .synchronization2 = VK_TRUE,
        .pNext = &deviceFeatures
    };

    VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pQueueCreateInfos = queueCreateInfo,
        .queueCreateInfoCount = queueNumber,
        .pEnabledFeatures = NULL,
        .enabledExtensionCount = deviceExtensionsCount,
        .ppEnabledExtensionNames = deviceExtensions,
        .pNext = &features13,
        .flags = 0,
    };

    MY_ASSERT(indexingFeature.shaderSampledImageArrayNonUniformIndexing);
    MY_ASSERT(indexingFeature.descriptorBindingSampledImageUpdateAfterBind);
    MY_ASSERT(indexingFeature.shaderUniformBufferArrayNonUniformIndexing);
    // MY_ASSERT(indexingFeature.descriptorBindingUniformBufferUpdateAfterBind); // it is probably unnecessary
    MY_ASSERT(indexingFeature.shaderStorageBufferArrayNonUniformIndexing);
    MY_ASSERT(indexingFeature.descriptorBindingStorageBufferUpdateAfterBind);
    MY_ASSERT(VK_SUCCESS == vkCreateDevice(physicalDevice, &createInfo, NULL, &device));

    return device;
}
