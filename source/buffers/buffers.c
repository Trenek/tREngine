#include <vulkan/vulkan.h>

#include "bufferOperations.h"

#include "definitions.h"

void createBuffers(VkBufferUsageFlagBits bufferUsageBit, VkDeviceSize bufferSize, VkBuffer *uniformBuffers, VkDeviceMemory *uniformBuffersMemory, void *uniformBuffersMapped[], VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    *uniformBuffers = createBuffer(
        device,
        physicalDevice,
        surface,
        bufferSize * MAX_FRAMES_IN_FLIGHT,
        bufferUsageBit
    );
    *uniformBuffersMemory = createBufferMemory(
        device,
        physicalDevice,
        *uniformBuffers,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    vkMapMemory(device, *uniformBuffersMemory, 0, bufferSize, 0, uniformBuffersMapped);

    for (size_t i = 1; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        uniformBuffersMapped[i] = (char *)uniformBuffersMapped[0] + i * bufferSize;
    }
}
