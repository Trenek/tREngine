#include <cglm.h>

#include "entity.h"
#include "graphicsSetup.h"
#include "modelBuilder.h"
#include "actualModel.h"

struct toCleanup {
    VkDevice device;

    struct buffer anim;
};

void cleanupAnim(void *toCleanArg) {
    struct toCleanup *toClean = toCleanArg;

    if (toClean) {
        destroyBuffers(toClean->device, toClean->anim.buffers, toClean->anim.buffersMemory);

        free(toClean);
    }
}

struct Entity *createModel(struct ModelBuilder builder, struct GraphicsSetup *graphics) {
    VkBuffer (*buff[])[MAX_FRAMES_IN_FLIGHT] = {
        &builder.modelData->localMesh.buffers,
        NULL
    };

    bool isChangable[] = {
        false,
        true
    };

    void *(*mapp[])[MAX_FRAMES_IN_FLIGHT] = {
        &builder.modelData->localMesh.buffersMapped,
        NULL
    };

    size_t range[] = {
        builder.modelData->meshQuantity * sizeof(mat4),
        builder.modelData->qNode * sizeof(mat4)
    };

    struct toCleanup *anim = malloc(sizeof(struct toCleanup));

    anim->device = graphics->device;
    createBuffers(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, range[1], anim->anim.buffers, anim->anim.buffersMemory, anim->anim.buffersMapped, graphics->device, graphics->physicalDevice, graphics->surface);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        for (size_t j = 0; j < builder.modelData->qNode; j += 1) {
            glm_mat4_identity(((mat4**)anim->anim.buffersMapped)[i][j]);
        }
    }

    buff[1] = &anim->anim.buffers;
    mapp[1] = &anim->anim.buffersMapped;

    size_t qBuff = 2 - (buff[1] == NULL);

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

        .additional = anim,
        .cleanup = cleanupAnim,
    }, graphics);
}
