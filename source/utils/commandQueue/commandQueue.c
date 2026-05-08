#include <vulkan/vulkan.h>

#include "graphicsSetup.h"
#include "graphicsFunctions.h"

#include "commandQueue.h"

void createCommandBuffer(VkCommandBuffer commandBuffer[MAX_FRAMES_IN_FLIGHT], VkDevice device, VkCommandPool commandPool);

void createCommandQueue(struct CommandQueue *this, struct GraphicsSetup *graphics) {
    createCommandBuffer(this->commandBuffer, graphics->device, graphics->commandPool);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        this->inFlightFence[i] = createFence(graphics->device);
        this->semaphore[i] = createSemaphore(graphics->device);
    }
}

void destroyCommandQueue(struct CommandQueue *this, struct GraphicsSetup *graphics) {
    vkDeviceWaitIdle(graphics->device);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        vkDestroyFence(graphics->device, this->inFlightFence[i], NULL);
        vkDestroySemaphore(graphics->device, this->semaphore[i], NULL);
    }
}

void recreateCommandQueue(struct CommandQueue *this, struct GraphicsSetup *graphics) {
    vkDeviceWaitIdle(graphics->device);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        vkDestroyFence(graphics->device, this->inFlightFence[i], NULL);
        vkDestroySemaphore(graphics->device, this->semaphore[i], NULL);
    }

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        this->inFlightFence[i] = createFence(graphics->device);
        this->semaphore[i] = createSemaphore(graphics->device);
    }
}
