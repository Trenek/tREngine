#include <vulkan/vulkan.h>
#include <string.h>

#include "camera.h"
#include "cameraBufferObject.h"

void updateFirstPersonCameraBuffer(void *uniformBuffersMapped, VkExtent2D swapChainExtent, void *cameraPtr) {
    struct FirstPerson *camera = cameraPtr;
    struct CameraBuffer ubo;

    ubo.lightColor[0] = 1;
    ubo.lightColor[1] = 1;
    ubo.lightColor[2] = 1;
    ubo.lightDirection[0] = 1;
    ubo.lightDirection[1] = 1;
    ubo.lightDirection[2] = 1;

    memcpy(ubo.cameraPos, camera->pos, sizeof(vec3));
    glm_look_rh_no(camera->pos, camera->direction, (vec3) { 0.0f, 1.0f, 0.0f }, ubo.view);
    glm_perspective(glm_rad(45.0f), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 10000.0f, ubo.proj);

    ubo.proj[1][1] *= -1;

    memcpy(uniformBuffersMapped, &ubo, sizeof(ubo));
}

void updateThirdPersonCameraBuffer(void *uniformBuffersMapped, VkExtent2D swapChainExtent, void *cameraPtr) {
    struct ThirdPerson *camera = cameraPtr;
    struct CameraBuffer ubo;

    ubo.lightColor[0] = 1;
    ubo.lightColor[1] = 1;
    ubo.lightColor[2] = 1;
    ubo.lightColor[3] = 0;
    ubo.lightDirection[0] = 1;
    ubo.lightDirection[1] = 1;
    ubo.lightDirection[2] = 1;
    ubo.lightDirection[3] = 0;

    glm_vec3_add(camera->center, camera->relativePos, ubo.cameraPos);

    glm_lookat_rh_no(camera->relativePos, camera->center, (vec3) { 0.0f, 1.0f, 0.0f }, ubo.view);
    glm_perspective(glm_rad(70.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10000.0f, ubo.proj);

    ubo.proj[1][1] *= -1;

    memcpy(uniformBuffersMapped, &ubo, sizeof(ubo));
}
