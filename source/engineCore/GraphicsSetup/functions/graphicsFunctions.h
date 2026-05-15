#include "queueFamilyIndices.h"

// setup
VkInstance createInstance(VkDebugUtilsMessengerEXT *debugMessenger);
VkSurfaceKHR createSurface(GLFWwindow *window, VkInstance instance);
VkPhysicalDevice pickPhysicalDevice(VkSampleCountFlagBits *msaaSamples, VkInstance instance, VkSurfaceKHR surface);
VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, struct QueueFamilyIndices indices);
struct swapChain createSwapChain(GLFWwindow *window, VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, VkDevice device);
VkImageView *createImageViews(VkDevice device, struct swapChain swapChain);

VkCommandPool createCommandPool(VkDevice device, struct QueueFamilyIndices indices);
VkCommandPool createTransferCommandPool(VkDevice device, struct QueueFamilyIndices indices);

void createColorResources(VkImage *colorImage, VkDeviceMemory *colorImageMemory, VkImageView *colorImageView, VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D swapChainExtent, VkFormat swapChainImageFormat, VkSampleCountFlagBits msaaSamples);
void createDepthResources(VkImage *depthImage, VkDeviceMemory *depthImageMemory, VkImageView *depthImageView, VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D swapChainExtent, VkSampleCountFlagBits msaaSamples, VkCommandPool commandPool, VkQueue queue);

VkSemaphore createSemaphore(VkDevice device);
VkFence createFence(VkDevice device);

// cleanup
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);
void destroyImageViews(VkImageView *swapChainImageViews, uint32_t swapChainImagesCount, VkDevice device);
void freeSwapChain(VkDevice device, struct swapChain *swapChain);
