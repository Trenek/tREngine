#include <GLFW/glfw3.h>

#include "graphicsFunctions.h"
#include "graphicsSetup.h"

static void cleanupSwapChain(struct GraphicsSetup *graphics) {
    for (size_t i = 0; i < graphics->swapChain.imagesCount; i += 1) {
        vkDestroySemaphore(graphics->device, graphics->swapChain.renderFinishedSemaphore[i], NULL);
    }

    vkDestroyImageView(graphics->device, graphics->colorImageView, NULL);
    vkDestroyImage(graphics->device, graphics->colorImage, NULL);
    vkFreeMemory(graphics->device, graphics->colorImageMemory, NULL);

    vkDestroyImageView(graphics->device, graphics->depthImageView, NULL);
    vkDestroyImage(graphics->device, graphics->depthImage, NULL);
    vkFreeMemory(graphics->device, graphics->depthImageMemory, NULL);

    destroyImageViews(graphics->swapChainImageViews, graphics->swapChain.imagesCount, graphics->device);

    vkDestroySwapchainKHR(graphics->device, graphics->swapChain.this, NULL);
}

void recreateSwapChainGraphics(GLFWwindow *window, struct GraphicsSetup *graphics) {
    vkDeviceWaitIdle(graphics->device);

    cleanupSwapChain(graphics);

    graphics->swapChain = createSwapChain(window, graphics->surface, graphics->physicalDevice, graphics->device);
    graphics->swapChainImageViews = createImageViews(graphics->device, graphics->swapChain);

    createColorResources(&graphics->colorImage, &graphics->colorImageMemory, &graphics->colorImageView, graphics->device, graphics->physicalDevice, graphics->swapChain.extent, graphics->swapChain.imageFormat, graphics->msaaSamples);
    createDepthResources(&graphics->depthImage, &graphics->depthImageMemory, &graphics->depthImageView, graphics->device, graphics->physicalDevice, graphics->swapChain.extent, graphics->msaaSamples, graphics->transferCommandPool, graphics->transferQueue);
}

struct GraphicsSetup setupGraphics(GLFWwindow *window) {
    struct GraphicsSetup graphics = { 0 };

    graphics.instance = createInstance(&graphics.debugMessenger);
    graphics.surface = createSurface(window, graphics.instance);
    graphics.physicalDevice = pickPhysicalDevice(&graphics.msaaSamples, graphics.instance, graphics.surface);
    graphics.families = findQueueFamilies(graphics.physicalDevice, graphics.surface);
    graphics.device = createLogicalDevice(graphics.physicalDevice, graphics.families);

    vkGetDeviceQueue(graphics.device, graphics.families.family[GRAPHICS_FAMILY].value, 0, &graphics.graphicsQueue);
    vkGetDeviceQueue(graphics.device, graphics.families.family[PRESENT_FAMILY].value, 0, &graphics.presentQueue);
    vkGetDeviceQueue(graphics.device, graphics.families.family[TRANSFER_FAMILY].value, 0, &graphics.transferQueue);
    vkGetDeviceQueue(graphics.device, graphics.families.family[COMPUTE_FAMILY].value, 0, &graphics.computeQueue);

    graphics.swapChain = createSwapChain(window, graphics.surface, graphics.physicalDevice, graphics.device);
    graphics.swapChainImageViews = createImageViews(graphics.device, graphics.swapChain);

    graphics.commandPool = createCommandPool(graphics.device, graphics.families);
    graphics.transferCommandPool = createTransferCommandPool(graphics.device, graphics.families);

    createColorResources(&graphics.colorImage, &graphics.colorImageMemory, &graphics.colorImageView, graphics.device, graphics.physicalDevice, graphics.swapChain.extent, graphics.swapChain.imageFormat, graphics.msaaSamples);
    createDepthResources(&graphics.depthImage, &graphics.depthImageMemory, &graphics.depthImageView, graphics.device, graphics.physicalDevice, graphics.swapChain.extent, graphics.msaaSamples, graphics.transferCommandPool, graphics.transferQueue);

    return graphics;
}

void cleanupGraphics(struct GraphicsSetup graphics) {
    vkDeviceWaitIdle(graphics.device);

    vkDestroyImageView(graphics.device, graphics.colorImageView, NULL);
    vkDestroyImage(graphics.device, graphics.colorImage, NULL);
    vkFreeMemory(graphics.device, graphics.colorImageMemory, NULL);

    vkDestroyImageView(graphics.device, graphics.depthImageView, NULL);
    vkDestroyImage(graphics.device, graphics.depthImage, NULL);
    vkFreeMemory(graphics.device, graphics.depthImageMemory, NULL);

    vkDestroyCommandPool(graphics.device, graphics.commandPool, NULL);
    vkDestroyCommandPool(graphics.device, graphics.transferCommandPool, NULL);

    destroyImageViews(graphics.swapChainImageViews, graphics.swapChain.imagesCount, graphics.device);
    freeSwapChain(graphics.device, &graphics.swapChain);
    vkDestroyDevice(graphics.device, NULL);
    DestroyDebugUtilsMessengerEXT(graphics.instance, graphics.debugMessenger, NULL);
    vkDestroySurfaceKHR(graphics.instance, graphics.surface, NULL);
    vkDestroyInstance(graphics.instance, NULL);
}
