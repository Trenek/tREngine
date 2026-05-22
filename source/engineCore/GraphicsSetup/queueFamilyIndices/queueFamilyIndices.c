#include "queueFamilyIndices.h"

bool familyEqual(struct Family family1, struct Family family2) {
    return family1.exists == family2.exists && family1.value == family2.value;
}

inline static struct Family setFamily(uint32_t i) {
    return (struct Family) {
        .exists = true,
        .value = i
    };
}

bool isFamilyVal(struct Family family, uint32_t i) {
    return family.exists == true && family.value == i;
}

struct QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
    struct QueueFamilyIndices indices = { 0 };

    uint32_t queueFamilyCount = 0; {
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
    }
    VkQueueFamilyProperties queueFamilies[queueFamilyCount];

    VkBool32 presentSupport = false;
    bool found = false;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for (size_t i = 0; i < queueFamilyCount && found == false; i += 1) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.family[GRAPHICS_FAMILY] = setFamily(i);
        }
        if (queueFamilies[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)) {
            indices.family[COMPUTE_FAMILY] = setFamily(i);
        }

        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport) {
            indices.family[PRESENT_FAMILY] = setFamily(i);
        }

        if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            indices.family[TRANSFER_FAMILY] = setFamily(i);
        }

        found = isFamilyVal(indices.family[GRAPHICS_FAMILY], i) &&
                isFamilyVal(indices.family[PRESENT_FAMILY], i) &&
                isFamilyVal(indices.family[TRANSFER_FAMILY], i) &&
                isFamilyVal(indices.family[COMPUTE_FAMILY], i);
    }

    return indices;
}
