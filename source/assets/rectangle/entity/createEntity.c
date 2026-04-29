#include <cglm/cglm.h>

#include "entity.h"
#include "model.h"

#include "rectangleBuilder.h"
#include "rectangle.h"

struct Entity *createRec(struct RecBuilder builder, struct GraphicsSetup *graphics) {
    return createEntity((struct EntityBuilder) {
        .instanceCount = builder.instanceCount,
        .meshQuantity = builder.modelData->meshQuantity,
        .mesh = builder.modelData->mesh,

        .objectLayout = builder.objectLayout,
        .instance = builder.instance,
    }, graphics);
}
