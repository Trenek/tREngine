#ifndef BUFFER_OBJ_H
#define BUFFER_OBJ_H

#include <vulkan/vulkan_core.h>

struct BufferObj {
    VkDevice device;

    VkBuffer buffer;
    VkDeviceMemory memory;

    VkDeviceSize range;
    VkDeviceSize actualSize;
    VkDeviceSize repetitons;
    VkDeviceSize value;
};

struct BufferBuilder {
    VkDeviceSize size;
    VkDeviceSize repetitions;

    VkBufferUsageFlagBits bufferUsage;
    VkMemoryPropertyFlagBits memoryProperty;

    VkDeviceSize value;
};

struct GraphicsSetup;
struct BufferObj *createBufferObj(struct BufferBuilder builder, struct GraphicsSetup *graphics);
void destroyBufferObj(void *bufferPtr);

void copyBufferObj(struct BufferObj * restrict dst, struct BufferObj * restrict src, size_t qBufferCopy, VkBufferCopy bc[qBufferCopy], struct GraphicsSetup * restrict graphicsSetup);

#endif
