#include <vulkan/vulkan.h>
#include <string.h>

#include "camera.h"
#include "cameraBufferObject.h"

void updateFirstPersonCameraBuffer(void *uniformBuffersMapped, VkExtent2D swapChainExtent, union camera camera) {
    struct CameraBuffer ubo;

    glm_look_rh_no(camera.fP.pos, camera.fP.direction, (vec3) { 0.0f, 0.0f, 1.0f }, ubo.view);
    glm_perspective(glm_rad(45.0f), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 10000.0f, ubo.proj);

    ubo.proj[1][1] *= -1;

    memcpy(uniformBuffersMapped, &ubo, sizeof(ubo));
}

void updateThirdPersonCameraBuffer(void *uniformBuffersMapped, VkExtent2D swapChainExtent, union camera camera) {
    struct CameraBuffer ubo;

    glm_lookat_rh_no(camera.tP.relativePos, camera.tP.center, (vec3) { 0.0f, 0.0f, 1.0f }, ubo.view);
    glm_perspective(glm_rad(70.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10000.0f, ubo.proj);

    ubo.proj[1][1] *= -1;

    memcpy(uniformBuffersMapped, &ubo, sizeof(ubo));
}
