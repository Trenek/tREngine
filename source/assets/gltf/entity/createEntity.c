#include <cglm/cglm.h>
#include <string.h>

#include "entity.h"
#include "model.h"
#include "graphicsSetup.h"

#include "gltfBuilder.h"
#include "gltf.h"

#include "buffer.h"
#include "bufferOperations.h"

struct toCleanup {
    VkDevice device;

    struct buffer anim;
};

void cleanupAnim(void *toCleanArg) {
    struct toCleanup *toClean = toCleanArg;

    if (toClean) {
        destroyBuffer(toClean->device, toClean->anim.buffers, toClean->anim.buffersMemory);

        free(toClean);
    }
}

struct Entity *createGltf(struct GltfBuilder builder, struct GraphicsSetup *graphics) {
    struct GltfModelInfo *modelInfo = builder.modelData->info;

    VkBuffer (*buff[]) = {
        &modelInfo->buffers[0].buffers,
        NULL,
        &modelInfo->buffers[2].buffers,
    };

    void *(*mapp[])[MAX_FRAMES_IN_FLIGHT] = {
        &modelInfo->buffers[0].buffersMapped,
        NULL,
        &modelInfo->buffers[2].buffersMapped,
    };

    bool isChangable[] = {
        false,
        true,
        false,
    };

    size_t range[] = {
        modelInfo->buffers[0].range,
        modelInfo->buffers[1].range,
        modelInfo->buffers[2].range,
    };

    struct toCleanup *anim = malloc(sizeof(struct toCleanup));

    anim->device = graphics->device;
    createBuffers(
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 
        range[1], 
        &anim->anim.buffers, 
        &anim->anim.buffersMemory, 
        anim->anim.buffersMapped, 
        graphics->device, 
        graphics->physicalDevice, 
        graphics->surface
    );

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        memcpy(
            anim->anim.buffersMapped[i],
            modelInfo->buffers[1].buffersMapped[i],
            modelInfo->buffers[1].range
        );
    }

    buff[1] = &anim->anim.buffers;
    mapp[1] = &anim->anim.buffersMapped;

    size_t qBuff = sizeof(buff) / sizeof(buff[0]);
    return createEntity((struct EntityBuilder) {
        .instanceCount = builder.instanceCount,
        .meshQuantity = builder.modelData->meshQuantity,
        .mesh = builder.modelData->mesh,

        .buff = buff,
        .mapp = mapp,
        .isChangable = isChangable,
        .range = range,
        .qBuff = qBuff,

        .objectLayout = builder.objectLayout,

        .instanceSize = builder.instanceSize,
        .instanceBufferSize = builder.instanceBufferSize,
        .instanceUpdater = builder.instanceUpdater,

        .destination = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        .pushConstantsSize = sizeof(struct GltfPushConstants),
        .pushConstants = modelInfo->pushConstants,

        .additional = anim,
        .cleanup = cleanupAnim,
    }, graphics);
}
