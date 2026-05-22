#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "graphicsSetup.h"

#include "model.h"
#include "entity.h"
#include "entityBuilder.h"
#include "bufferObj.h"

struct InstanceInfo {
    VkDevice device;
    struct BufferObj *instanceBuffer;
    void *instanceMapped[MAX_FRAMES_IN_FLIGHT];

    void *prevInfo;
    void (*prevCleanup)(void *);
};

static void cleanupInstance(void *infoPtr) {
    struct InstanceInfo *info = infoPtr;

    destroyBufferObj(info->instanceBuffer);

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
    VkBuffer buff[qBuff];
    void *(*mapp[qBuff])[MAX_FRAMES_IN_FLIGHT];
    bool isSingle[qBuff];
    size_t range[qBuff];

    info->instanceBuffer = createBufferObj((struct BufferBuilder) {
        .bufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        .memoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        .size = builder.instanceCount * builder.instance.bufferSize,
        .repetitions = MAX_FRAMES_IN_FLIGHT
    }, graphics);

    vkMapMemory(graphics->device, info->instanceBuffer->memory, 0, info->instanceBuffer->range, 0, &info->instanceMapped[0]);
    for (size_t i = 1; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        info->instanceMapped[i] = (char *)info->instanceMapped[i - 1] + info->instanceBuffer->range;
    }

    mapp[0] = &info->instanceMapped;
    buff[0] = info->instanceBuffer->buffer;
    range[0] = info->instanceBuffer->range;
    isSingle[0] = false;

    if (builder.qBuff > 0) {
        memcpy(buff + 1, builder.buff, sizeof(void *) * builder.qBuff);
        memcpy(range + 1, builder.range, sizeof(size_t) * builder.qBuff);
        memcpy(mapp + 1, builder.mapp, sizeof(void *) * builder.qBuff);
        memcpy(isSingle + 1, builder.isSingle, sizeof(bool) * builder.qBuff);
    }

    builder.qBuff = qBuff;
    builder.buff = buff;
    builder.mapp = mapp;
    builder.isSingle = isSingle;
    builder.range = range;
    builder.additional = info;
    builder.cleanup = cleanupInstance;

    return createEntity(builder, graphics);
}

size_t countChangeableBuffers(size_t qBuff, void *(*mapp[qBuff])[MAX_FRAMES_IN_FLIGHT]) {
    size_t result = 0;

    for (size_t i = 0; i < qBuff; i += 1) {
        result += mapp[i] != NULL;
    }

    return result;
}

struct Entity *createEntity(struct EntityBuilder builder, struct GraphicsSetup *graphics) {
    struct Entity *result = calloc(1, sizeof(struct Entity));
    size_t qBuff = countChangeableBuffers(builder.qBuff, builder.mapp);

    *result = (struct Entity){
        .device = graphics->device,
        .additional = builder.additional,
        .cleanup = builder.cleanup,

        .instanceCount = builder.instanceCount,
        .instance = malloc(builder.instance.size * builder.instanceCount),
        .instanceUpdater = builder.instance.updater,

        .qBuff = qBuff,
        .buffer = calloc(qBuff, sizeof(void *)),
        .range = calloc(qBuff, sizeof(size_t)),
        .mapp = calloc(qBuff, sizeof(void *)),

        .drawCallQuantity = builder.meshQuantity,
        .drawCall = calloc(builder.meshQuantity, sizeof(struct DrawCall)),
    };

    if (builder.qBuff > 0) {
        for (size_t i = 0, j = 0; i < builder.qBuff; i += 1) {
            if (NULL != builder.mapp[i]) {
                result->buffer[j] = calloc(1, builder.range[i]);
                memcpy(result->buffer[j], (*builder.mapp[i])[0], builder.range[i]);
                result->range[j] = builder.range[i];
                result->mapp[j] = builder.mapp[i];

                j += 1;
            }
        }

        result->object = createDescriptorSetsObj(graphics, &(struct DescriptorObjBuilder) {
            .layout = builder.objectLayout,
            .qDescriptorPoolSize = 1,
            .descriptorPoolSize = (VkDescriptorPoolSize []) {
                {
                    .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                    .descriptorCount = MAX_FRAMES_IN_FLIGHT * builder.qBuff
                }
            },
        });

        bindBuffersToDescriptorSets(result->object, graphics->device, builder.qBuff, builder.buff, builder.range, builder.isSingle, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    }

    for (size_t i = 0; i < builder.meshQuantity; i += 1) {
        ((struct DrawCall *)result->drawCall)[i] = (struct DrawCall) {
            .vertexBuffer = builder.mesh[i].vertex->buffer,

            .indicesQuantity = builder.mesh[i].indicesQuantity,
            .indexBuffer = builder.mesh[i].index->buffer,

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

    if (model->object) {
        destroyDescriptorSets(model->object);
    }

    free(model);
}

void destroyEntityArray(size_t num, struct Entity *modelArray[num], struct GraphicsSetup *graphics) {
    vkDeviceWaitIdle(graphics->device);

    for (size_t i = 0; i < num; i += 1) {
        destroyEntity(modelArray[i]);
    }
}
