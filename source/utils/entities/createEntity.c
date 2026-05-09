#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "graphicsSetup.h"

#include "model.h"
#include "entity.h"
#include "entityBuilder.h"
#include "bufferOperations.h"

struct InstanceInfo {
    VkDevice device;
    struct buffer uniformModel;

    void *prevInfo;
    void (*prevCleanup)(void *);
};

static void cleanupInstance(void *infoPtr) {
    struct InstanceInfo *info = infoPtr;
    destroyBuffer(info->device, info->uniformModel.buffers, info->uniformModel.buffersMemory);

    if (info->prevCleanup) {
        info->prevCleanup(info->prevInfo);
    }

    free(info);
}

struct Entity *createInstancedEntity(struct EntityBuilder builder, struct GraphicsSetup *graphics) {
    struct InstanceInfo *info = calloc(1, sizeof(struct InstanceInfo));
    *info = (struct InstanceInfo) {
        .device = graphics->device,
        .prevCleanup = builder.cleanup,
        .prevInfo = builder.additional
    };

    size_t qBuff = builder.qBuff + 1;
    VkBuffer (*buff[qBuff]);
    void *(*mapp[qBuff])[MAX_FRAMES_IN_FLIGHT];
    bool isChangable[qBuff];
    size_t range[qBuff];

    createBuffers(
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 
        builder.instanceCount * builder.instance.bufferSize, 
        &info->uniformModel.buffers, 
        &info->uniformModel.buffersMemory, 
        info->uniformModel.buffersMapped, 
        graphics->device, 
        graphics->physicalDevice, 
        graphics->surface
    );

    mapp[0] = &info->uniformModel.buffersMapped;
    buff[0] = &info->uniformModel.buffers;
    range[0] = builder.instanceCount * builder.instance.bufferSize;
    isChangable[0] = true;
    memcpy(buff + 1, builder.buff, sizeof(void *) * builder.qBuff);
    memcpy(range + 1, builder.range, sizeof(size_t) * builder.qBuff);
    memcpy(mapp + 1, builder.mapp, sizeof(void *) * builder.qBuff);
    memcpy(isChangable + 1, builder.isChangable, sizeof(bool) * builder.qBuff);

    builder.qBuff = qBuff;
    builder.buff = buff;
    builder.mapp = mapp;
    builder.isChangable = isChangable;
    builder.range = range;
    builder.additional = info;
    builder.cleanup = cleanupInstance;

    return createEntity(builder, graphics);
}

struct Entity *createEntity(struct EntityBuilder builder, struct GraphicsSetup *graphics) {
    struct Entity *result = calloc(1, sizeof(struct Entity));

    *result = (struct Entity){
        .device = graphics->device,
        .additional = builder.additional,
        .cleanup = builder.cleanup,

        .instanceCount = builder.instanceCount,
        .instance = malloc(builder.instance.size * builder.instanceCount),
        .instanceUpdater = builder.instance.updater,

        .buffer = calloc(builder.qBuff, sizeof(void *)),
        .range = calloc(builder.qBuff, sizeof(size_t)),
        .mapp = calloc(builder.qBuff, sizeof(void *)),

        .drawCallQuantity = builder.meshQuantity,
        .drawCall = calloc(builder.meshQuantity, sizeof(struct DrawCall)),

        .object.descriptorPool = builder.qBuff > 0 ? createDescriptorPool(graphics->device, builder.qBuff, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) : NULL,
        .qBuff = builder.qBuff,
    };

    if (builder.qBuff > 0) {
        for (size_t i = 0; i < builder.qBuff; i += 1) {
            result->buffer[i] = builder.isChangable[i] ? calloc(1, builder.range[i]) : NULL;
            if (builder.isChangable[i]) {
                memcpy(result->buffer[i], (*builder.mapp[i])[0], builder.range[i]);
            }
        }

        VkBuffer (*buff2[builder.qBuff]);

        memcpy(buff2, builder.buff, sizeof(void *) * builder.qBuff);
        memcpy(result->range, builder.range, sizeof(size_t) * builder.qBuff);
        memcpy(result->mapp, builder.mapp, sizeof(void *) * builder.qBuff);

        createDescriptorSets(result->object.descriptorSets, graphics->device, result->object.descriptorPool, builder.objectLayout);

        bindBuffersToDescriptorSets(result->object.descriptorSets, graphics->device, builder.qBuff, buff2, result->range, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    }

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

    vkDestroyDescriptorPool(model->device, model->object.descriptorPool, NULL);

    free(model);
}

void destroyEntityArray(size_t num, struct Entity *modelArray[num], struct GraphicsSetup *graphics) {
    vkDeviceWaitIdle(graphics->device);

    for (size_t i = 0; i < num; i += 1) {
        destroyEntity(modelArray[i]);
    }
}
