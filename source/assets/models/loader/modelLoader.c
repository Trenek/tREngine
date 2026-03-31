#include <string.h>

#include "entityBuilder.h"
#include "graphicsSetup.h"
#include "modelLoader.h"

#include "bufferOperations.h"

#include "MY_ASSERT.h"

struct Model *loadModel(const char *filePath, struct GraphicsSetup *graphics) {
    struct Model *result = calloc(1, sizeof(struct Model));
    struct ModelInput input = {};

    void (*fun)(const char *, struct ModelInput *, VkDevice, VkPhysicalDevice, VkSurfaceKHR) =
        NULL != strstr(filePath, ".ttf") ? ttfLoadModel :
        NULL;
    assert(NULL != fun);

    fun(filePath, &input, graphics->device, graphics->physicalDevice, graphics->surface);

    result->device = graphics->device;
    result->localMesh = input.localMesh;
    result->meshQuantity = input.meshQuantity;
    result->mesh = malloc(sizeof(struct Mesh) * result->meshQuantity);

    for (uint32_t i = 0; i < result->meshQuantity; i += 1) {
        result->mesh[i].indicesQuantity = input.mesh[i].indicesQuantity;
        result->mesh[i].vertexBuffer = createVertexBuffer(&result->mesh[i].vertexBufferMemory, graphics->device, graphics->physicalDevice, graphics->surface, graphics->commandPool, graphics->transferQueue, input.mesh[i].verticesQuantity, input.mesh[i].vertices, input.mesh[i].sizeOfVertex);
        result->mesh[i].indexBuffer = createIndexBuffer(&result->mesh[i].indexBufferMemory, graphics->device, graphics->physicalDevice, graphics->surface, graphics->commandPool, graphics->transferQueue, input.mesh[i].verticesQuantity, input.mesh[i].indicesQuantity, input.mesh[i].indices, input.mesh[i].sizeOfVertex);

        free(input.mesh[i].vertices);
        free(input.mesh[i].indices);
    }

    free(input.mesh);

    return result;
}

void destroyActualModel(void *modelPtr) {
    struct Model *model = modelPtr;

    for (uint32_t j = 0; j < model->meshQuantity; j += 1) {
        destroyBuffer(model->device, model->mesh[j].indexBuffer, model->mesh[j].indexBufferMemory);
        destroyBuffer(model->device, model->mesh[j].vertexBuffer, model->mesh[j].vertexBufferMemory);
    }

    destroyBuffers(model->device, model->localMesh.buffers, model->localMesh.buffersMemory);

    free(model);
}
