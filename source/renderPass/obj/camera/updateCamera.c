#include <vulkan/vulkan.h>
#include <string.h>

#include "camera.h"
#include "cameraBufferObject.h"

void updateFirstPersonCameraBuffer(void *uniformBuffersMapped, VkExtent2D swapChainExtent, union camera camera) {
    struct CameraBuffer ubo;

    ubo.lightColor[0] = 1;
    ubo.lightColor[1] = 1;
    ubo.lightColor[2] = 1;
    ubo.lightDirection[0] = 1;
    ubo.lightDirection[1] = 1;
    ubo.lightDirection[2] = 1;

    memcpy(ubo.cameraPos, camera.fP.pos, sizeof(vec3));
    glm_look_rh_no(camera.fP.pos, camera.fP.direction, (vec3) { 0.0f, 1.0f, 0.0f }, ubo.view);
    glm_perspective(glm_rad(45.0f), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 10000.0f, ubo.proj);

    ubo.proj[1][1] *= -1;

    memcpy(uniformBuffersMapped, &ubo, sizeof(ubo));
}

void updateThirdPersonCameraBuffer(void *uniformBuffersMapped, VkExtent2D swapChainExtent, union camera camera) {
    struct CameraBuffer ubo;

    ubo.lightColor[0] = 1;
    ubo.lightColor[1] = 1;
    ubo.lightColor[2] = 1;
    ubo.lightColor[3] = 0;
    ubo.lightDirection[0] = 1;
    ubo.lightDirection[1] = 1;
    ubo.lightDirection[2] = 1;
    ubo.lightDirection[3] = 0;

    glm_vec3_add(camera.tP.center, camera.tP.relativePos, ubo.cameraPos);
    printf("%f %f %f\r", ubo.cameraPos[0], ubo.cameraPos[1], ubo.cameraPos[2]);

    glm_lookat_rh_no(camera.tP.relativePos, camera.tP.center, (vec3) { 0.0f, 1.0f, 0.0f }, ubo.view);
    glm_perspective(glm_rad(70.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10000.0f, ubo.proj);

    ubo.proj[1][1] *= -1;

    memcpy(uniformBuffersMapped, &ubo, sizeof(ubo));
}
