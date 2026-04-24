#ifndef RENDER_PASS_OBJ_H
#define RENDER_PASS_OBJ_H

#include <vulkan/vulkan.h>

#include "definitions.h"
#include "defaultCamera.h"
#include "buffer.h"

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
    struct renderPassCore *renderPass;

    size_t cameraSize;
    size_t cameraBufferSize;
    void *camera;
    void (*updateCameraBuffer)(void *buffersMapped, VkExtent2D swapChainExtent, void *);

    struct pipelineConnection *data;
    size_t qData;

    VkDescriptorSetLayout cameraDescriptorSetLayout;
};

struct renderPassObj {
    VkDevice device;
    double coordinates[4];
    double color[4];
    
    struct renderPassCore *renderPass;

    struct pipelineConnection *data;
    size_t qData;

    void *camera;
    void (*updateCameraBuffer)(void *buffersMapped, VkExtent2D swapChainExtent, void *);

    struct buffer cameraBuffer;
    VkDescriptorPool cameraDescriptorPool;
    VkDescriptorSet cameraDescriptorSet[MAX_FRAMES_IN_FLIGHT];
};

struct renderPassObj *createRenderPassObj(struct renderPassBuilder builder, struct GraphicsSetup *graphics);
void destroyRenderPassObj(void *renderPassPtr);
void destroyRenderPassObjArr(size_t qRenderPass, struct renderPassObj **renderPass);

#endif
