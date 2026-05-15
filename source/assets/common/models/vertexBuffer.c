#include <string.h>

#include <vulkan/vulkan.h>

#include "MY_ASSERT.h"
#include "graphicsSetup.h"
#include "bufferObj.h"

struct BufferObj *createVertexBuffer(struct GraphicsSetup *graphics, uint32_t vertexQuantity, void *vertices, size_t sizeOfBuffer) {
    struct BufferObj *staging = createBufferObj((struct BufferBuilder) {
        .bufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .memoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        .size = vertexQuantity * sizeOfBuffer,
        .repetitions = 1,
    }, graphics);

    void *data = NULL;

    vkMapMemory(graphics->device, staging->memory, 0, vertexQuantity * sizeOfBuffer, 0, &data);
    memcpy(data, vertices, vertexQuantity * sizeOfBuffer);
    vkUnmapMemory(graphics->device, staging->memory);

    struct BufferObj *vertex = createBufferObj((struct BufferBuilder) {
        .bufferUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | 
                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        .size = vertexQuantity * sizeOfBuffer,
        .repetitions = 1,
    }, graphics);

    copyBufferObj(vertex, staging, 1, (VkBufferCopy []) {
        {
            .size = vertexQuantity * sizeOfBuffer
        }
    }, graphics);

    destroyBufferObj(staging);

    return vertex;
}

struct BufferObj *createIndexBuffer(struct GraphicsSetup *graphics, uint32_t indicesQuantity, uint32_t indices[static indicesQuantity]) {
    size_t sizeOfIndex = sizeof(uint32_t);

    struct BufferObj *staging = createBufferObj((struct BufferBuilder) {
        .bufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .memoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        .size = indicesQuantity * sizeOfIndex,
        .repetitions = 1,
    }, graphics);

    void *data = NULL;

    vkMapMemory(graphics->device, staging->memory, 0, indicesQuantity * sizeOfIndex, 0, &data);
    memcpy(data, indices, indicesQuantity * sizeOfIndex);
    vkUnmapMemory(graphics->device, staging->memory);

    struct BufferObj *index = createBufferObj((struct BufferBuilder) {
        .bufferUsage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | 
                       VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        .memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        .size = indicesQuantity * sizeOfIndex,
        .repetitions = 1,
    }, graphics);

    copyBufferObj(index, staging, 1, (VkBufferCopy []) {
        {
            .size = indicesQuantity * sizeOfIndex
        }
    }, graphics);

    destroyBufferObj(staging);

    return index;
}
