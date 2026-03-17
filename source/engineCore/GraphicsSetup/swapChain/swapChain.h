#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <vulkan/vulkan.h>

struct swapChain {
    VkSemaphore *renderFinishedSemaphore;
    VkImage *images;
    uint32_t imagesCount;
    VkFormat imageFormat;
    VkExtent2D extent;
    VkSwapchainKHR this;
};

#endif
