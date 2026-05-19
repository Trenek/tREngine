#include <vulkan/vulkan.h>

#include "imageOperations.h"
#include "depthHelpers.h"

#include "MY_ASSERT.h"

void createColorResources(VkImage *colorImage, VkDeviceMemory *colorImageMemory, VkImageView *colorImageView, VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D swapChainExtent, VkFormat swapChainImageFormat, VkSampleCountFlagBits msaaSamples) {
    MY_ASSERT(VK_SUCCESS == vkCreateImage(device, &(VkImageCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .extent = {
            .width = swapChainExtent.width,
            .height = swapChainExtent.height,
            .depth = 1
        },
        .mipLevels = 1,
        .arrayLayers = 1,
        .format = swapChainImageFormat,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
                 VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .samples = msaaSamples,
        .flags = 0
    }, NULL, colorImage));

    *colorImageMemory = createImageMemory(device, physicalDevice, *colorImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    *colorImageView = createImageView(device, *colorImage, swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, VK_IMAGE_VIEW_TYPE_2D, 1);
}
