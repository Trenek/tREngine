#include <string.h>
#include <stdlib.h>

#include <cglm/cglm.h>

#include "graphicsSetup.h"

#include "model.h"
#include "fontBuilder.h"
#include "entity.h"
#include "entityBuilder.h"

#include "bufferObj.h"
#include "font.h"

size_t getGlyphID(char a);

static size_t count(const char *buffer) {
    size_t i = 0;

    while (*buffer != 0) {
        i += *buffer != ' ';

        buffer += 1;
    }

    return i;
}

static float getStringLength(const char *string, float *offset) {
    float length = 0;

    while (*string != 0) {
        length += offset[getGlyphID(*string)];

        string += 1;
    }

    return length;
}

struct toCleanup {
    VkDevice device;

    struct Mesh *mesh;
    void *pushConstants;

    struct BufferObj *localMesh;
};

static void cleanupFont(void *toCleanArg) {
    struct toCleanup *toClean = toCleanArg;

    destroyBufferObj(toClean->localMesh);

    free(toClean->mesh);
    free(toClean->pushConstants);

    free(toClean);
}

struct Entity *createFont(struct FontBuilder builder, struct GraphicsSetup *graphics) {
    uint32_t meshQuantity = count(builder.string);

    struct FontModelInfo *modelInfo = builder.modelData->info;
    struct toCleanup *info = malloc(sizeof(struct toCleanup));
    *info = (struct toCleanup) {
        .device = graphics->device,
        .mesh = malloc(sizeof(struct Mesh) * meshQuantity),
        .pushConstants = malloc(sizeof(struct FontPushConstants) * meshQuantity),
    };

    struct FontPushConstants *pc = info->pushConstants;

    struct BufferObj *staging = createBufferObj((struct BufferBuilder) {
        .bufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .memoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        .size = meshQuantity * sizeof(mat4),
        .repetitions = MAX_FRAMES_IN_FLIGHT
    }, graphics);
    void *mapped[MAX_FRAMES_IN_FLIGHT];

    vkMapMemory(graphics->device, staging->memory, 0, staging->range, 0, mapped);

    for (size_t i = 1; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        mapped[i] = (char *)mapped[i - 1] + staging->range;
    }

    mat4 **transformation = (void *)mapped;

    const char *string = builder.string;
    float offset = -getStringLength(string, modelInfo->offset) * (builder.center + 1) / 2;

    for (size_t i = 0; i < meshQuantity; i += 1) {
        pc[i].meshID = i;

        while (*string == ' ') {
            offset += modelInfo->offset[getGlyphID(' ')];

            string += 1;
        }

        info->mesh[i] = builder.modelData->mesh[getGlyphID(*string)];

        for (uint32_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j += 1) {
            glm_mat4_identity(transformation[j][i]);
            glm_translate_x(transformation[j][i], offset);
        }

        offset += modelInfo->offset[getGlyphID(*string)];
        string += 1;
    }

    vkUnmapMemory(graphics->device, staging->memory);

    info->localMesh = createBufferObj((struct BufferBuilder) {
        .bufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                       VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        .memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        .size = meshQuantity * sizeof(mat4),
        .repetitions = MAX_FRAMES_IN_FLIGHT
    }, graphics);

    VkBufferCopy copies[MAX_FRAMES_IN_FLIGHT]; for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        copies[i] = (VkBufferCopy) {
            .size = staging->range,
            .dstOffset = i * info->localMesh->range,
            .srcOffset = i * staging->range
        };
    }

    copyBufferObj(info->localMesh, staging, MAX_FRAMES_IN_FLIGHT, copies, graphics);
    destroyBufferObj(staging);

    VkBuffer buff[] = {
        info->localMesh->buffer
    };

    bool isChangable[] = {
        false,
    };

    bool isSingle[] = {
        false,
    };

    void *(*mapp[])[MAX_FRAMES_IN_FLIGHT] = {
        NULL
    };

    size_t range[] = {
        info->localMesh->range
    };

    return createInstancedEntity((struct EntityBuilder) {
        .meshQuantity = meshQuantity,
        .mesh = info->mesh,

        .buff = buff,
        .mapp = mapp,
        .isChangable = isChangable,
        .isSingle = isSingle,
        .range = range,
        .qBuff = 1,

        .instanceCount = builder.instanceCount,
        .objectLayout = builder.objectLayout,
        .instance = builder.instance,

        .destination = VK_SHADER_STAGE_VERTEX_BIT,
        .pushConstantsSize = sizeof(struct FontPushConstants),
        .pushConstants = info->pushConstants,

        .additional = info,
        .cleanup = cleanupFont
    }, graphics);
}
