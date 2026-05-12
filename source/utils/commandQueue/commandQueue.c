#include <stdlib.h>

#include <vulkan/vulkan.h>

#include "graphicsSetup.h"
#include "graphicsFunctions.h"

#include "commandQueue.h"

void createCommandBuffer(VkCommandBuffer commandBuffer[MAX_FRAMES_IN_FLIGHT], VkDevice device, VkCommandPool commandPool);

struct CommandQueue *createCommandQueue(struct GraphicsSetup *graphics) {
    struct CommandQueue *this = malloc(sizeof(struct CommandQueue));
    createCommandBuffer(this->commandBuffer, graphics->device, graphics->commandPool);

    this->device = graphics->device;
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        this->inFlightFence[i] = createFence(graphics->device);
        this->semaphore[i] = createSemaphore(graphics->device);
    }

    return this;
}

void destroyCommandQueue(void *thisPtr) {
    struct CommandQueue *this = thisPtr;
    vkDeviceWaitIdle(this->device);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        vkDestroyFence(this->device, this->inFlightFence[i], NULL);
        vkDestroySemaphore(this->device, this->semaphore[i], NULL);
    }

    free(this);
}

void recreateCommandQueue(struct CommandQueue *this) {
    vkDeviceWaitIdle(this->device);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        vkDestroyFence(this->device, this->inFlightFence[i], NULL);
        vkDestroySemaphore(this->device, this->semaphore[i], NULL);
    }

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        this->inFlightFence[i] = createFence(this->device);
        this->semaphore[i] = createSemaphore(this->device);
    }
}
