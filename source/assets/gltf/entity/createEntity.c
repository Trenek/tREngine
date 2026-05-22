#include <cglm/cglm.h>
#include <string.h>

#include "entity.h"
#include "model.h"
#include "graphicsSetup.h"

#include "gltfBuilder.h"
#include "gltf.h"

#include "bufferObj.h"

struct toCleanup {
    VkDevice device;

    struct BufferObj *anim;
    void *animMapped[MAX_FRAMES_IN_FLIGHT];
};

void cleanupAnim(void *toCleanArg) {
    struct toCleanup *toClean = toCleanArg;

    destroyBufferObj(toClean->anim);

    free(toClean);
}

struct Entity *createGltf(struct GltfBuilder builder, struct GraphicsSetup *graphics) {
    struct GltfModelInfo *modelInfo = builder.modelData->info;

    VkBuffer buff[] = {
        modelInfo->buffers[0]->buffer,
        modelInfo->buffers[1]->buffer,
        NULL,
    };

    void *(*mapp[])[MAX_FRAMES_IN_FLIGHT] = {
        NULL,
        NULL,
        NULL,
    };

    bool isSingle[] = {
        true,
        true,
        false
    };

    size_t range[] = {
        modelInfo->buffers[0]->range,
        modelInfo->buffers[1]->range,
        modelInfo->qNodes * sizeof(struct AnimationData),
    };

    struct toCleanup *anim = malloc(sizeof(struct toCleanup));

    anim->device = graphics->device;
    anim->anim = createBufferObj((struct BufferBuilder) {
        .bufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        .memoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        .size = modelInfo->qNodes * sizeof(struct AnimationData),
        .repetitions = MAX_FRAMES_IN_FLIGHT
    }, graphics);

    vkMapMemory(graphics->device, anim->anim->memory, 0, anim->anim->range, 0, anim->animMapped);
    for (size_t i = 1; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        anim->animMapped[i] = (char *)anim->animMapped[i - 1] + anim->anim->range;
    }

    buff[2] = anim->anim->buffer;
    mapp[2] = &anim->animMapped;

    size_t qBuff = sizeof(buff) / sizeof(buff[0]);
    return createInstancedEntity((struct EntityBuilder) {
        .instanceCount = builder.instanceCount,
        .meshQuantity = builder.modelData->meshQuantity,
        .mesh = builder.modelData->mesh,

        .buff = buff,
        .mapp = mapp,
        .isSingle = isSingle,
        .range = range,
        .qBuff = qBuff,

        .objectLayout = builder.objectLayout,
        .instance = builder.instance,

        .destination = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        .pushConstantsSize = sizeof(struct GltfPushConstants),
        .pushConstants = modelInfo->pushConstants,

        .additional = anim,
        .cleanup = cleanupAnim,
    }, graphics);
}
