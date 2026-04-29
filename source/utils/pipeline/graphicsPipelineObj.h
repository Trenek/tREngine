#ifndef GRAPHICS_PIPELINE_OBJ_H
#define GRAPHICS_PIPELINE_OBJ_H

#include <vulkan/vulkan_core.h>
#include "Vertex.h"

struct GraphicsSetup;

struct graphicsPipelineBuilder {
    size_t qRenderPassCore;
    struct renderPassCore **renderPassCore;

    const char *vertexShader;
    const char *fragmentShader;

    const float minDepth;
    const float maxDepth;

    VkPrimitiveTopology topology;

    struct Vert vert;

    VkCullModeFlags cullFlags;

    VkCompareOp operation;

    VkPipelineLayout pipelineLayout;
};

struct renderPipeline {
    VkPipeline pipeline;
    struct renderPassCore *core;
};

struct graphicsPipeline {
    VkDevice device;

    VkPipelineLayout pipelineLayout;
    size_t qPipelines;
    struct renderPipeline *pipeline;
};

struct graphicsPipeline *createObjGraphicsPipeline(struct graphicsPipelineBuilder builder, struct GraphicsSetup *graphics);
void destroyObjGraphicsPipeline(void *pipePtr);

#endif
