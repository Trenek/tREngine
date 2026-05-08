#ifndef GRAPHICS_SETUP_H
#define GRAPHICS_SETUP_H

#include <vulkan/vulkan.h>

#include "swapChain.h"
#include "queueFamilyIndices.h"

struct GraphicsSetup {
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkQueue presentQueue;
    VkQueue graphicsQueue;
    VkQueue transferQueue;
    VkQueue computeQueue;
    VkDevice device;

    struct QueueFamilyIndices families;

    struct swapChain swapChain;
    VkImageView *swapChainImageViews;

    VkCommandPool commandPool;
    VkCommandPool transferCommandPool;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    VkSampleCountFlagBits msaaSamples;

    VkImage colorImage;
    VkDeviceMemory colorImageMemory;
    VkImageView colorImageView;
};

typedef struct GLFWwindow GLFWwindow;

void recreateSwapChainGraphics(GLFWwindow *window, struct GraphicsSetup *vulkan);
struct GraphicsSetup setupGraphics(GLFWwindow *window);
void cleanupGraphics(struct GraphicsSetup vulkan);

#endif
