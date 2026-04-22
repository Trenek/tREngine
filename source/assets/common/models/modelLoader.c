#include <string.h>
#include <stdlib.h>

#include "buffer.h"

#include "entityBuilder.h"
#include "graphicsSetup.h"
#include "modelLoader.h"

#include "bufferOperations.h"

#include "MY_ASSERT.h"

#include "texture.h"

struct Model *loadModel(const char *filePath, struct GraphicsSetup *graphics) {
    struct Model *result = calloc(1, sizeof(struct Model));
    struct ModelInput input = {};

    void (*fun)(const char *, struct ModelInput *, struct GraphicsSetup *) =
        NULL != strstr(filePath, ".ttf") ? ttfLoadModel :
        NULL != strstr(filePath, ".obj") ? objLoadModel :
        NULL != strstr(filePath, ".glb") ? gltfLoadModel :
        NULL != strstr(filePath, ".gltf") ? gltfLoadModel :
        NULL != strstr(filePath, ".rec") ? recLoadModel :
        NULL;
    assert(NULL != fun);

    fun(filePath, &input, graphics);

    result->device = graphics->device;
    result->cleanup = input.cleanup;
    result->info = input.info;
    result->qTexture = input.qTexture;
    result->texture = input.texture;
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

    for (uint32_t i = 0; i < model->meshQuantity; i += 1) {
        destroyBuffer(model->device, model->mesh[i].indexBuffer, model->mesh[i].indexBufferMemory);
        destroyBuffer(model->device, model->mesh[i].vertexBuffer, model->mesh[i].vertexBufferMemory);
    }

    for (size_t i = 0; i < model->qTexture; i += 1) {
        free(model->texture[i].data);
    }
    free(model->texture);

    model->cleanup(model->info);
    free(model->mesh);
    free(model);
}
