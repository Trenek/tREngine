#include <vulkan/vulkan.h>

#include "definitions.h"

struct CommandQueue {
    VkCommandBuffer commandBuffer[MAX_FRAMES_IN_FLIGHT];
    VkFence inFlightFence[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore semaphore[MAX_FRAMES_IN_FLIGHT];
};

struct GraphicsSetup;

void createCommandQueue(struct CommandQueue *this, struct GraphicsSetup *graphics);
void destroyCommandQueue(struct CommandQueue *this, struct GraphicsSetup *graphics);
void recreateCommandQueue(struct CommandQueue *this, struct GraphicsSetup *graphics);
