#ifndef RENDER_PASS_OBJ_H
#define RENDER_PASS_OBJ_H

#include <vulkan/vulkan.h>

#include "definitions.h"
#include "cameraBuilder.h"

struct Entity;
struct Pipeline;
struct GraphicsSetup;

struct BuffersToUpdate {
    void *buffer;
    void **mapp;
    size_t range;
};

struct pipelineConnection {
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;

    VkDescriptorSet *texture;

    VkDescriptorSet **entitySet;
    size_t qEntity;

    void **drawData;
    size_t *qDrawData;
};

struct pipelineConnectionBuilder {
    size_t pipeNum;
    struct Pipeline *pipe;
    VkDescriptorSet *texture;
    struct Entity **entity;
    size_t qEntity;
};

struct renderPassObj;
struct renderPassBuilder {
    double coordinates[4];
    double color[4];
    struct renderPassCore *renderPass;

    struct cameraBuilder camera;
    struct pipelineConnectionBuilder *data;
    size_t qData;

    VkDescriptorSetLayout cameraDescriptorSetLayout;
    void (*drawRenderPass)(VkCommandBuffer, uint32_t, struct renderPassObj *);
};

struct renderPassObj {
    VkDevice device;
    double coordinates[4];
    double color[4];
    
    struct renderPassCore *renderPass;

    struct pipelineConnection *data;
    size_t qData;

    size_t qBuffersToUpdate;
    struct BuffersToUpdate *buffersToUpdate;

    void *camera;
    void (*updateCameraBuffer)(void *buffersMapped, VkExtent2D swapChainExtent, void *);

    void *cameraMapped[MAX_FRAMES_IN_FLIGHT];
    struct BufferObj *cameraBuffer;
    struct DescriptorObj *cameraDescriptor;

    void (*drawRenderPass)(VkCommandBuffer, uint32_t, struct renderPassObj *);
};

struct renderPassObj *createRenderPassObj(struct renderPassBuilder builder, struct GraphicsSetup *graphics);
void destroyRenderPassObj(void *renderPassPtr);
void destroyRenderPassObjArr(size_t qRenderPass, struct renderPassObj **renderPass);

void drawRenderPass(VkCommandBuffer commandBuffer, uint32_t currentFrame, struct renderPassObj *renderPass);
void drawRenderPassComp(VkCommandBuffer commandBuffer, uint32_t currentFrame, struct renderPassObj *renderPass);

#endif
