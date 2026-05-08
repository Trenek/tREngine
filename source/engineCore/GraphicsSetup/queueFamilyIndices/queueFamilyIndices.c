#include "queueFamilyIndices.h"

bool familyEqual(struct Family family1, struct Family family2) {
    return family1.exists == family2.exists && family1.value == family2.value;
}

struct QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
    struct QueueFamilyIndices indices = { 0 };

    uint32_t queueFamilyCount = 0; {
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
    }
    VkQueueFamilyProperties queueFamilies[queueFamilyCount];

    VkBool32 presentSupport = false;
    bool found = false;

    uint32_t i = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    while (i < queueFamilyCount && found == false) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.family[GRAPHICS_FAMILY].value = i;
            indices.family[GRAPHICS_FAMILY].exists = true;
        }
        if (queueFamilies[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)) {
            indices.family[COMPUTE_FAMILY].value = i;
            indices.family[COMPUTE_FAMILY].exists = true;
        }

        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport) {
            indices.family[PRESENT_FAMILY].value = i;
            indices.family[PRESENT_FAMILY].exists = true;
        }

        if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            indices.family[TRANSFER_FAMILY].value = i;
            indices.family[TRANSFER_FAMILY].exists = true;
        }

        found = indices.family[GRAPHICS_FAMILY].exists &&
                indices.family[PRESENT_FAMILY].exists &&
                indices.family[TRANSFER_FAMILY].exists &&
                indices.family[COMPUTE_FAMILY].exists &&
                indices.family[GRAPHICS_FAMILY].value == i &&
                indices.family[PRESENT_FAMILY].value == i &&
                indices.family[TRANSFER_FAMILY].value == i &&
                indices.family[COMPUTE_FAMILY].value == i;

        i += 1;
    }

    return indices;
}
