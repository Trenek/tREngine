#include <vulkan/vulkan.h>
#include <myMalloc.h>

#include "MY_ASSERT.h"
#include "swapChain.h"
#include "imageOperations.h"
#include "swapChainSupportDetails.h"

VkImageView *createImageViews(VkDevice device, struct swapChain swapChain) {
    VkImageView *swapChainImageViews = malloc(sizeof(VkImageView) * swapChain.imagesCount);

    for (size_t i = 0; i < swapChain.imagesCount; i += 1) {
        MY_ASSERT(VK_SUCCESS == vkCreateImageView(device, &(VkImageViewCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapChain.images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = swapChain.imageFormat,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        }, NULL, &swapChainImageViews[i]));
    }

    return swapChainImageViews;
}

void destroyImageViews(VkImageView *swapChainImageViews, uint32_t swapChainImagesCount, VkDevice device) {
    for (size_t i = 0; i < swapChainImagesCount; i += 1) {
        vkDestroyImageView(device, swapChainImageViews[i], NULL);
    }

    free(swapChainImageViews);
}
