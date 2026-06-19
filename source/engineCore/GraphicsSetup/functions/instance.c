#include <myMalloc.h>
#include <string.h>
#include <stdio.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "MY_ASSERT.h"

#ifdef NDEBUG
static constexpr bool enableValidationLayers = false;
#else
static constexpr bool enableValidationLayers = true;
#endif

static const char *const layers[] = {
#ifndef NDEBUG
    "VK_LAYER_KHRONOS_validation",
#endif
    NULL,
};
static const uint32_t layersCount = sizeof(layers) / sizeof(const char *) - 1;

static const char *const extensions[] = {
#ifndef NDEBUG
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
    NULL,
};
static constexpr uint32_t extensionCount = sizeof(extensions) / sizeof(const char *) - 1;

static bool checkLayerSupport(void) {
    bool layerFound = true;

    uint32_t layerCount = 0; {
        vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    }
    VkLayerProperties avaibleLayers[layerCount];

    vkEnumerateInstanceLayerProperties(&layerCount, avaibleLayers);

    for (uint32_t i = 0; i < layersCount && layerFound == true; i += 1) {
        layerFound = false;

        for (uint32_t j = 0; j < layerCount && layerFound == false; j += 1) {
            layerFound = strcmp(layers[i], avaibleLayers[j].layerName) == 0;
        }
    }

    return layerFound;
}

[[maybe_unused]]
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *) {
    fprintf(stderr, "VL ");

    switch ((int)messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            fprintf(stderr, "VERBOSE: ");
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            fprintf(stderr, "INFO: ");
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            fprintf(stderr, "WARNING: ");
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            fprintf(stderr, "ERROR: ");
            break;
    }

    fprintf(stderr, "%s\n", pCallbackData->pMessage);

    return VK_FALSE;
}

[[maybe_unused]]
static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger) {
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    return func != NULL ? func(instance, pCreateInfo, pAllocator, pDebugMessenger) :
                          VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    if (func != NULL && enableValidationLayers) {
        func(instance, debugMessenger, pAllocator);
    }
}

static const char **getRequiredExtensions(uint32_t *outExtensionCount) {
    uint32_t qGlfwExtensions = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&qGlfwExtensions);

    uint32_t newSize = qGlfwExtensions + extensionCount;
    
    const char **realExtensions = malloc(sizeof(const char *) * newSize);

    memcpy(realExtensions, glfwExtensions, sizeof(const char *) * qGlfwExtensions);
    memcpy(realExtensions + qGlfwExtensions, extensions, sizeof(const char *) * extensionCount);
    
    *outExtensionCount = newSize;

    return realExtensions;
}

VkInstance createInstance([[maybe_unused]] VkDebugUtilsMessengerEXT *debugMessenger) {
    MY_ASSERT(true == checkLayerSupport());

    VkInstance instance = NULL;

    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = "Hello Triangle",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_3
    };

    uint32_t realExtensionCount = 0;
    const char **realExtensions = getRequiredExtensions(&realExtensionCount);
    
    [[maybe_unused]]
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity =  VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
//                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType =      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debugCallback,
        .flags = 0,
        .pNext = NULL,
        .pUserData = NULL
    };

    VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = enableValidationLayers ? &debugCreateInfo : NULL,
        .flags = 0,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = layersCount,
        .ppEnabledLayerNames = layers,
        .enabledExtensionCount = realExtensionCount,
        .ppEnabledExtensionNames = realExtensions
    };

    MY_ASSERT(VK_SUCCESS == vkCreateInstance(&createInfo, NULL, &instance));
    assert(VK_SUCCESS == CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, NULL, debugMessenger));

    if (enableValidationLayers) free(realExtensions);

    return instance;
}
