#ifndef CAMERA_H
#define CAMERA_H

#include <cglm.h>

union camera {
    struct FirstPerson {
        vec3 pos;
        vec3 direction;
    } fP;
    struct ThirdPerson {
        vec3 center;
        vec3 relativePos;
    } tP;
};

struct WindowManager;
typedef struct VkExtent2D VkExtent2D;

void moveThirdPersonCamera(struct WindowManager *windowControl, union camera *camera, float deltaTime);

void updateFirstPersonCameraBuffer(void *uniformBuffersMapped, VkExtent2D swapChainExtent, union camera camera);
void updateThirdPersonCameraBuffer(void *uniformBuffersMapped, VkExtent2D swapChainExtent, union camera camera);

#endif
