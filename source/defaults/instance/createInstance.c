#include "instanceBuffer.h"

#include "entity.h"

void instanceUpdater(void *instancePtr, void *instanceBufferPtr, uint32_t instanceCount, float deltaTime) {
    struct instance *instance = instancePtr;
    struct instanceBuffer *instanceBuffer = instanceBufferPtr;
    static float time = 0;
    time += deltaTime;

    for (uint32_t i = 0; i < instanceCount; i += 1) {
        glm_mat4_identity(instanceBuffer[i].modelMatrix);

        glm_translate(instanceBuffer[i].modelMatrix, instance[i].pos);
        glm_rotate(instanceBuffer[i].modelMatrix, instance[i].fixedRotation[0] + time * instance[i].rotation[0], (vec3) { 1, 0, 0 });
        glm_rotate(instanceBuffer[i].modelMatrix, instance[i].fixedRotation[1] + time * instance[i].rotation[1], (vec3) { 0, 1, 0 });
        glm_rotate(instanceBuffer[i].modelMatrix, instance[i].fixedRotation[2] + time * instance[i].rotation[2], (vec3) { 0, 0, 1 });
        glm_scale(instanceBuffer[i].modelMatrix, instance[i].scale);
        instanceBuffer[i].textureIndex = instance[i].textureIndex;
    }
}

void updateInstances(struct Entity **model, size_t qModel, float deltaTime) {
    for (uint32_t i = 0; i < qModel; i += 1) {
        model[i]->instanceUpdater(model[i]->instance, model[i]->buffer[0], model[i]->instanceCount, deltaTime);
    }
}
