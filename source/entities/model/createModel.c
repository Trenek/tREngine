#include <cglm/cglm.h>

#include "entity.h"
#include "graphicsSetup.h"
#include "modelBuilder.h"
#include "actualModel.h"

struct Entity *createModel(struct ModelBuilder builder, struct GraphicsSetup *graphics) {
    return createEntity((struct EntityBuilder) {
        .instanceCount = builder.instanceCount,
        .meshQuantity = builder.modelData->meshQuantity,
        .mesh = builder.modelData->mesh,

        .objectLayout = builder.objectLayout,

        .instanceSize = builder.instanceSize,
        .instanceBufferSize = builder.instanceBufferSize,
        .instanceUpdater = builder.instanceUpdater,
    }, graphics);
}
