#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

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

void moveThirdPersonCamera(struct WindowManager *windowControl, struct ThirdPerson *camera, float deltaTime);

void updateFirstPersonCameraBuffer(void *uniformBuffersMapped, VkExtent2D swapChainExtent, struct FirstPerson camera);
void updateThirdPersonCameraBuffer(void *uniformBuffersMapped, VkExtent2D swapChainExtent, struct ThirdPerson camera);

#endif
