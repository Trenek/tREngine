#include <string.h>
#include <stdlib.h>

#include "entityBuilder.h"
#include "graphicsSetup.h"
#include "modelLoader.h"
#include "model.h"

#include "bufferObj.h"

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
        NULL != strstr(filePath, ".scr") ? screenLoadModel :
        NULL;
    assert(NULL != fun);

    fun(filePath, &input, graphics);

    *result = (struct Model) {
        .device = graphics->device,
        .cleanup = input.cleanup,
        .info = input.info,
        .qTexture = input.qTexture,
        .texture = input.texture,
        .meshQuantity = input.meshQuantity,
        .mesh = malloc(sizeof(struct Mesh) * input.meshQuantity)
    };

    for (uint32_t i = 0; i < result->meshQuantity; i += 1) {
        result->mesh[i].indicesQuantity = input.mesh[i].indicesQuantity;
        result->mesh[i].vertex = createVertexBuffer(graphics, input.mesh[i].verticesQuantity, input.mesh[i].vertices, input.mesh[i].sizeOfVertex);
        result->mesh[i].index = createIndexBuffer(graphics, input.mesh[i].indicesQuantity, input.mesh[i].indices);

        free(input.mesh[i].vertices);
        free(input.mesh[i].indices);
    }

    free(input.mesh);

    return result;
}

void destroyActualModel(void *modelPtr) {
    struct Model *model = modelPtr;

    for (uint32_t i = 0; i < model->meshQuantity; i += 1) {
        destroyBufferObj(model->mesh[i].index);
        destroyBufferObj(model->mesh[i].vertex);
    }

    for (size_t i = 0; i < model->qTexture; i += 1) {
        free(model->texture[i].data);
    }
    free(model->texture);

    if (model->cleanup) {
        model->cleanup(model->info);
    }

    free(model->mesh);
    free(model);
}
