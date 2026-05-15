#include <vulkan/vulkan_core.h>
#include <malloc.h>

#include "bufferObj.h"

#include "graphicsSetup.h"
#include "MY_ASSERT.h"
#include "queueFamilyIndices.h"
#include "commonOperations.h"

static VkDeviceSize getAligment(VkBufferCreateInfo *bufferInfo, struct GraphicsSetup *graphics) {
    VkDeviceBufferMemoryRequirements info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_BUFFER_MEMORY_REQUIREMENTS,
        .pCreateInfo = bufferInfo
    };

    VkMemoryRequirements2 memRequirements2 = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2
    };

    vkGetDeviceBufferMemoryRequirements(graphics->device, &info, &memRequirements2);
    VkDeviceSize alignment = memRequirements2.memoryRequirements.alignment;

    VkDeviceSize allocSize = (bufferInfo->size / alignment) * alignment;

    return allocSize < bufferInfo->size ? allocSize + alignment : allocSize;
}

struct BufferObj *createBufferObj(struct BufferBuilder builder, struct GraphicsSetup *graphics) {
    struct BufferObj *bufferObj = malloc(sizeof(struct BufferObj));

    VkBufferCreateInfo bufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = builder.size,
        .usage = builder.bufferUsage,
        .sharingMode = familyEqual(graphics->families.family[GRAPHICS_FAMILY], graphics->families.family[TRANSFER_FAMILY]) ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount = 2,
        .pQueueFamilyIndices = (uint32_t[]){
            graphics->families.family[GRAPHICS_FAMILY].value,
            graphics->families.family[TRANSFER_FAMILY].value
        },
    };

    bufferObj->device = graphics->device;
    bufferObj->range = getAligment(&bufferCreateInfo, graphics);
    bufferObj->actualSize = builder.size;
    bufferObj->repetitons = builder.repetitions;
    bufferObj->value = builder.value;
    
    bufferCreateInfo.size = bufferObj->range * builder.repetitions;

    MY_ASSERT(VK_SUCCESS == vkCreateBuffer(graphics->device, &bufferCreateInfo, NULL, &bufferObj->buffer));

    VkMemoryRequirements memRequirements; {
        vkGetBufferMemoryRequirements(graphics->device, bufferObj->buffer, &memRequirements);
    }

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = findMemoryType(graphics->physicalDevice, memRequirements.memoryTypeBits, builder.memoryProperty)
    };

    // not supposet to call for every individual buffer, 4096 MAX, better to create custom allocator or use VulkanMemoryAllocator
    MY_ASSERT(VK_SUCCESS == vkAllocateMemory(graphics->device, &allocInfo, NULL, &bufferObj->memory));
    vkBindBufferMemory(graphics->device, bufferObj->buffer, bufferObj->memory, 0);

    return bufferObj;
}

void destroyBufferObj(void *bufferPtr) {
    struct BufferObj *buffer = bufferPtr;

    vkDestroyBuffer(buffer->device, buffer->buffer, NULL);
    vkFreeMemory(buffer->device, buffer->memory, NULL);

    free(buffer);
}

void copyBufferObj(struct BufferObj * restrict dst, struct BufferObj * restrict src, size_t qBufferCopy, VkBufferCopy bc[qBufferCopy], struct GraphicsSetup * restrict graphics) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(graphics->device, graphics->transferCommandPool);

    vkCmdCopyBuffer(commandBuffer, src->buffer, dst->buffer, qBufferCopy, bc);

    endSingleTimeCommands(commandBuffer, graphics->device, graphics->transferCommandPool, graphics->transferQueue);
}
