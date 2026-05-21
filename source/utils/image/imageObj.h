#ifndef BUFFER_OBJ_H
#define BUFFER_OBJ_H

#include <vulkan/vulkan_core.h>

struct ImageObj {
    VkDevice device;

    VkImage image;
    VkImageView imageView;
    VkDeviceMemory memory;
    VkSampler sampler;
};

struct ImageBuilder {
    VkExtent2D extent;
};

struct GraphicsSetup;
struct ImageObj *createImageObj(struct ImageBuilder builder, struct GraphicsSetup *graphics);
void destroyImageObj(void *bufferPtr);

#endif
