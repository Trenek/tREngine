#include <vulkan/vulkan.h>

#include "imageOperations.h"
#include "depthHelpers.h"

#include "MY_ASSERT.h"

void createDepthResources(VkImage *depthImage, VkDeviceMemory *depthImageMemory, VkImageView *depthImageView, VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D swapChainExtent, VkSampleCountFlagBits msaaSamples, VkCommandPool commandPool, VkQueue queue) {
    VkFormat depthFormat = findDepthFormat(physicalDevice);

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
        .format = depthFormat,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .samples = msaaSamples,
        .flags = 0
    }, NULL, depthImage));
    *depthImageMemory = createImageMemory(device, physicalDevice, *depthImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    *depthImageView = createImageView(device, *depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, VK_IMAGE_VIEW_TYPE_2D, 1);

    transitionImageLayout(*depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, device, commandPool, queue, 1);
}
