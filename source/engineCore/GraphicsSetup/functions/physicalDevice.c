#include <string.h>
#include <myMalloc.h>

#include <vulkan/vulkan.h>

#include "MY_ASSERT.h"
#include "swapChainSupportDetails.h"
#include "queueFamilyIndices.h"

const char *const deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
    VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
};
const size_t deviceExtensionsCount = sizeof(deviceExtensions) / sizeof(const char *const);

static bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
    bool isFound = true;

    uint32_t extensionCount = 0; {
        vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL);
    }
    VkExtensionProperties avaibleExtensions[extensionCount];

    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, avaibleExtensions);

    for (size_t i = 0; i < deviceExtensionsCount && isFound == true; i += 1) {
        isFound = false;

        for (size_t j = 0; isFound == false && j < extensionCount; j += 1) {
            isFound = strcmp(deviceExtensions[i], avaibleExtensions[j].extensionName) == 0;
        }
    }

    return isFound;
}

static int isDevicePreferable(VkPhysicalDevice device, VkSurfaceKHR surface) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    struct SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);

    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    freeSwapChainSupportDetails(&swapChainSupport);

    return
        (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 1000 : 0) +
        (deviceFeatures.geometryShader ? 1 : 0);
}

static bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    struct QueueFamilyIndices queueFamilies = findQueueFamilies(device, surface);
    struct SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);

    bool swapChainAdequate = swapChainSupport.formatCount != 0 &&
                             swapChainSupport.presentModeCount != 0;

    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    freeSwapChainSupportDetails(&swapChainSupport);
    
    return
//      deviceFeatures.depthBounds &&
        deviceFeatures.samplerAnisotropy &&
        checkDeviceExtensionSupport(device) &&
        swapChainAdequate &&
        queueFamilies.family[GRAPHICS_FAMILY].exists &&
        queueFamilies.family[PRESENT_FAMILY].exists;
}

static VkSampleCountFlagBits getMaxUsableSampleCount(VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceProperties physicalDeviceProperties; {
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    }
    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
                                physicalDeviceProperties.limits.framebufferDepthSampleCounts;

    return (counts & VK_SAMPLE_COUNT_64_BIT) ? VK_SAMPLE_COUNT_64_BIT :
           (counts & VK_SAMPLE_COUNT_32_BIT) ? VK_SAMPLE_COUNT_32_BIT :
           (counts & VK_SAMPLE_COUNT_16_BIT) ? VK_SAMPLE_COUNT_16_BIT :
           (counts & VK_SAMPLE_COUNT_8_BIT)  ? VK_SAMPLE_COUNT_8_BIT :
           (counts & VK_SAMPLE_COUNT_4_BIT)  ? VK_SAMPLE_COUNT_4_BIT :
           (counts & VK_SAMPLE_COUNT_2_BIT)  ? VK_SAMPLE_COUNT_2_BIT :
                                               VK_SAMPLE_COUNT_1_BIT;
}

VkPhysicalDevice pickPhysicalDevice(VkSampleCountFlagBits *msaaSamples, VkInstance instance, VkSurfaceKHR surface) {
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    uint32_t deviceCount = 0; {
        vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
        MY_ASSERT(deviceCount != 0);
    }
    VkPhysicalDevice devices[deviceCount];
    uint32_t preferableScore = 0;
    uint32_t currentScore = 0;

    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

    for (size_t i = 0; i < deviceCount; i += 1) {
        if (isDeviceSuitable(devices[i], surface)) {
            currentScore = isDevicePreferable(devices[i], surface);
            if (currentScore >= preferableScore) {
                preferableScore = currentScore;
                physicalDevice = devices[i];
            }
        }
    }

    MY_ASSERT(physicalDevice != NULL);
    *msaaSamples = getMaxUsableSampleCount(physicalDevice);

    return physicalDevice;
}
