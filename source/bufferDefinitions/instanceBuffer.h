#ifndef INSTANCE_H
#define INSTANCE_H

#include <cglm.h>

#define INS(x, y) \
    .instanceSize = sizeof(struct x), \
    .instanceBufferSize = sizeof(struct y), \
    .instanceUpdater = x##Updater

struct instanceBuffer {
    uint32_t textureIndex;
    mat4 modelMatrix;
};

struct instance {
    uint32_t textureIndex;
    vec3 pos;
    vec3 rotation;
    vec3 fixedRotation;
    vec3 scale;
};

struct Entity;
void updateInstances(struct Entity **model, size_t qModel, float deltaTime);
void instanceUpdater(void *instancePtr, void *instanceBufferPtr, uint32_t instanceCount, float deltaTime);
#endif
