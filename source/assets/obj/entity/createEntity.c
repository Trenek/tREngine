#include <cglm/cglm.h>

#include "entity.h"
#include "graphicsSetup.h"
#include "model.h"

#include "objBuilder.h"
#include "bufferObj.h"
#include "obj.h"

struct toCleanup {
    void *pushConstants;
};

static void cleanupPushConstants(void *toCleanArg) {
    struct toCleanup *toClean = toCleanArg;

    free(toClean->pushConstants);
    free(toClean);
}

struct Entity *createObj(struct ObjBuilder builder, struct GraphicsSetup *graphics) {
    struct toCleanup *info = malloc(sizeof(struct toCleanup));

    struct ObjPushConstants *pc = info->pushConstants = malloc(sizeof(struct ObjPushConstants) * builder.modelData->meshQuantity);

    struct ObjModelInfo *modelInfo = builder.modelData->info;

    VkBuffer buff[] = {
        modelInfo->buffer->buffer
    };

    void *(*mapp[])[MAX_FRAMES_IN_FLIGHT] = {
        NULL
    };

    bool isChangable[] = {
        false
    };

    bool isSingle[] = {
        true
    };

    size_t range[] = {
        modelInfo->buffer->range
    };

    size_t qBuff = 1;

    for (size_t i = 0; i < builder.modelData->meshQuantity; i += 1) {
        pc[i].textureOffset = builder.textureOffset;
    }

    return createInstancedEntity((struct EntityBuilder) {
        .instanceCount = builder.instanceCount,
        .meshQuantity = builder.modelData->meshQuantity,
        .mesh = builder.modelData->mesh,

        .buff = buff,
        .mapp = mapp,
        .isChangable = isChangable,
        .isSingle = isSingle,
        .range = range,
        .qBuff = qBuff,

        .objectLayout = builder.objectLayout,
        .instance = builder.instance,

        .destination = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pushConstantsSize = sizeof(struct ObjPushConstants),
        .pushConstants = info->pushConstants,

        .additional = info,
        .cleanup = cleanupPushConstants
    }, graphics);
}
