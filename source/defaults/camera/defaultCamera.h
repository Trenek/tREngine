#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

struct CameraBuffer {
    alignas(16) mat4 view;
    alignas(16) mat4 proj;

    vec4 lightDirection;
    vec4 lightColor;
    vec4 cameraPos;
    vec4 pad0;
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

#endif
