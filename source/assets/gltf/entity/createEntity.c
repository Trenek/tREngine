#include <cglm/cglm.h>

#include "entity.h"
#include "graphicsSetup.h"
#include "actualModel.h"

#include "gltfBuilder.h"
#include "gltf.h"

struct Entity *createGltf(struct GltfBuilder builder, struct GraphicsSetup *graphics) {
    struct GltfModelInfo *modelInfo = builder.modelData->info;

    VkBuffer (*buff[]) = {
        &modelInfo->buffers[0].buffers,
        &modelInfo->buffers[1].buffers,
        &modelInfo->buffers[2].buffers,
    };

    void *(*mapp[])[MAX_FRAMES_IN_FLIGHT] = {
        &modelInfo->buffers[0].buffersMapped,
        &modelInfo->buffers[1].buffersMapped,
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
    }, graphics);
}
