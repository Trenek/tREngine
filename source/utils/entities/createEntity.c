#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "graphicsSetup.h"

#include "model.h"
#include "entity.h"
#include "entityBuilder.h"
#include "bufferOperations.h"

struct Entity *createEntity(struct EntityBuilder builder, struct GraphicsSetup *graphics) {
    struct Entity *result = calloc(1, sizeof(struct Entity));

    *result = (struct Entity){
        .device = graphics->device,
        .additional = builder.additional,
        .cleanup = builder.cleanup,

        .instanceCount = builder.instanceCount,
        .instance = malloc(builder.instance.size * builder.instanceCount),
        .instanceUpdater = builder.instance.updater,

        .buffer = calloc(builder.qBuff + 1, sizeof(void *)),
        .range = calloc(builder.qBuff + 1, sizeof(size_t)),
        .mapp = calloc(builder.qBuff + 1, sizeof(void *)),

        .drawCallQuantity = builder.meshQuantity,
        .drawCall = calloc(builder.meshQuantity, sizeof(struct DrawCall)),

        .object.descriptorPool = createDescriptorPool(graphics->device, builder.qBuff + 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER),
        .qBuff = builder.qBuff + 1
    };

    result->buffer[0] = calloc(builder.instanceCount, builder.instance.bufferSize);
    for (size_t i = 0; i < builder.qBuff; i += 1) {
        result->buffer[i + 1] = builder.isChangable[i] ? calloc(1, builder.range[i]) : NULL;
        if (builder.isChangable[i]) {
            memcpy(result->buffer[i + 1], (*builder.mapp[i])[0], builder.range[i]);
        }
    }

    VkBuffer (*buff2[builder.qBuff + 1]);

    createBuffers(
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 
        builder.instanceCount * builder.instance.bufferSize, 
        &result->uniformModel.buffers, 
        &result->uniformModel.buffersMemory, 
        result->uniformModel.buffersMapped, 
        graphics->device, 
        graphics->physicalDevice, 
        graphics->surface
    );

    result->mapp[0] = &result->uniformModel.buffersMapped;
    buff2[0] = &result->uniformModel.buffers;
    result->range[0] = builder.instanceCount * builder.instance.bufferSize;
    memcpy(buff2 + 1, builder.buff, sizeof(void *) * builder.qBuff);
    memcpy(result->range + 1, builder.range, sizeof(size_t) * builder.qBuff);
    memcpy(result->mapp + 1, builder.mapp, sizeof(void *) * builder.qBuff);

    memset(result->buffer[0], 0, builder.instance.bufferSize * builder.instanceCount);

    createDescriptorSets(result->object.descriptorSets, graphics->device, result->object.descriptorPool, builder.objectLayout);

    bindBuffersToDescriptorSets(result->object.descriptorSets, graphics->device, builder.qBuff + 1, buff2, result->range, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

    for (size_t i = 0; i < builder.meshQuantity; i += 1) {
        ((struct DrawCall *)result->drawCall)[i] = (struct DrawCall) {
            .vertexBuffer = builder.mesh[i].vertexBuffer,

            .indicesQuantity = builder.mesh[i].indicesQuantity,
            .indexBuffer = builder.mesh[i].indexBuffer,

            .pushConstantsStage = builder.destination,
            .pushConstantsSize = builder.pushConstantsSize,
            .pushConstans = (char *)builder.pushConstants + i * builder.pushConstantsSize,

            .instanceCount = builder.instanceCount,
        };
    }

    return result;
}

void destroyEntity(void *modelPtr) {
    struct Entity *model = modelPtr;

    if (model->cleanup != NULL) {
        model->cleanup(model->additional);
    }

    for (size_t i = 0; i < model->qBuff; i += 1) {
        free(model->buffer[i]);
    }

    free(model->instance);
    free(model->buffer);
    free(model->range);
    free(model->mapp);
    free(model->drawCall);

    destroyBuffer(model->device, model->uniformModel.buffers, model->uniformModel.buffersMemory);

    vkDestroyDescriptorPool(model->device, model->object.descriptorPool, NULL);

    free(model);
}

void destroyEntityArray(size_t num, struct Entity *modelArray[num], struct GraphicsSetup *graphics) {
    vkDeviceWaitIdle(graphics->device);

    for (size_t i = 0; i < num; i += 1) {
        destroyEntity(modelArray[i]);
    }
}
