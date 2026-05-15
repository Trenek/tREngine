#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

#include "cameraBuilder.h"

struct CameraBuffer {
    alignas(16) mat4 view;
    alignas(16) mat4 proj;

    vec4 lightDirection;
    vec4 lightColor;
    vec4 cameraPos;
};

struct FirstPerson {
    vec3 pos;
    vec3 direction;
};

struct ThirdPerson {
    vec3 center;
    vec3 relativePos;
};

struct WindowManager;
typedef struct VkExtent2D VkExtent2D;
typedef struct VkDevice_T *VkDevice;

struct descriptorSetLayout *defaultCameraDescriptorSetLayout(VkDevice device);

void moveThirdPersonCamera(struct WindowManager *windowControl, struct ThirdPerson *camera, float deltaTime);
void moveFirstPersonCamera(struct WindowManager *windowControl, struct FirstPerson *cameraPtr, float deltaTime);

void updateFirstPersonCameraBuffer(void *uniformBuffersMapped, VkExtent2D swapChainExtent, void *camera);
void updateThirdPersonCameraBuffer(void *uniformBuffersMapped, VkExtent2D swapChainExtent, void *camera);

static inline struct cameraBuilder defaultFirstPersonCamera(const struct FirstPerson * const restrict data) {
    return (struct cameraBuilder) {
        .updateBuffer = updateFirstPersonCameraBuffer,
        .size = sizeof(struct FirstPerson),
        .bufferSize = sizeof(struct CameraBuffer),
        .mapped = data
    };
}
static inline struct cameraBuilder defaultThirdPersonCamera(const struct ThirdPerson * const restrict data) {
    return (struct cameraBuilder) {
        .updateBuffer = updateThirdPersonCameraBuffer,
        .size = sizeof(struct ThirdPerson),
        .bufferSize = sizeof(struct CameraBuffer),
        .mapped = data
    };
}

#endif
