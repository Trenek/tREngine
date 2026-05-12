#include <vulkan/vulkan.h>

#include "definitions.h"

struct CommandQueue {
    VkDevice device;

    VkCommandBuffer commandBuffer[MAX_FRAMES_IN_FLIGHT];
    VkFence inFlightFence[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore semaphore[MAX_FRAMES_IN_FLIGHT];
};

struct GraphicsSetup;

struct CommandQueue *createCommandQueue(struct GraphicsSetup *graphics);
void destroyCommandQueue(void *this);
void recreateCommandQueue(struct CommandQueue *this);
