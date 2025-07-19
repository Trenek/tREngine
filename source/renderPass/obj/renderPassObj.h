#ifndef RENDER_PASS_OBJ_H
#define RENDER_PASS_OBJ_H

#include <vulkan/vulkan.h>

#include "definitions.h"
#include "camera.h"

struct Entity;
struct graphicsPipeline;
struct GraphicsSetup;

struct pipelineConnection {
    size_t pipeNum;
    struct graphicsPipeline *pipe;
    struct descriptor *texture;
    struct Entity **entity;
    size_t qEntity;
};

struct renderPassBuilder {
    double coordinates[4];
    double color[4];
    union camera camera;
    struct renderPassCore *renderPass;
    
    void (*updateCameraBuffer)(void *buffersMapped, VkExtent2D swapChainExtent, union camera camera);

    struct pipelineConnection *data;
    size_t qData;
};

struct renderPassObj {
    VkDevice device;
    double coordinates[4];
    double color[4];
    
    struct renderPassCore *renderPass;
    union camera camera;
    void (*updateCameraBuffer)(void *buffersMapped, VkExtent2D swapChainExtent, union camera camera);

    struct pipelineConnection *data;
    size_t qData;

    VkBuffer cameraBuffer[MAX_FRAMES_IN_FLIGHT];
    VkDeviceMemory cameraBufferMemory[MAX_FRAMES_IN_FLIGHT];
    void *cameraBufferMapped[MAX_FRAMES_IN_FLIGHT];

    VkDescriptorSetLayout cameraDescriptorSetLayout;
    VkDescriptorPool cameraDescriptorPool;
    VkDescriptorSet cameraDescriptorSet[MAX_FRAMES_IN_FLIGHT];
};

struct renderPassObj *createRenderPassObj(struct renderPassBuilder builder, struct GraphicsSetup *graphics);
void destroyRenderPassObj(void *renderPassPtr);
void destroyRenderPassObjArr(size_t qRenderPass, struct renderPassObj **renderPass);

#endif
