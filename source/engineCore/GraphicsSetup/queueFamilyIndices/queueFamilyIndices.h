#ifndef QUEUE_FAMILY_INDICES_H
#define QUEUE_FAMILY_INDICES_H

#include <vulkan/vulkan.h>

struct Family {
    bool exists;
    uint32_t value;
};

enum {
    GRAPHICS_FAMILY,
    PRESENT_FAMILY,
    TRANSFER_FAMILY,
    COMPUTE_FAMILY,

    Q_QUEUE,
};

struct QueueFamilyIndices {
    struct Family family[Q_QUEUE];
};

bool familyEqual(struct Family family1, struct Family family2);
struct QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

#endif
