#include <malloc.h>

#include "graphicsPipelineObj.h"
#include "renderPassCore.h"

#include "pipelineFunctions.h"
#include "graphicsSetup.h"

struct Pipeline *createGraphicsPipelineObj(struct GraphicsPipelineBuilder builder, struct GraphicsSetup *graphics) {
    struct Pipeline *graphicsPipe = calloc(1, sizeof(struct Pipeline)); 

    *graphicsPipe = (struct Pipeline) {
        .device = graphics->device,
        .pipelineLayout = builder.pipelineLayout,
        .qPipelines = builder.qRenderPassCore,
        .pipeline = malloc(sizeof(struct renderPipeline) * builder.qRenderPassCore)
    };

    for (size_t i = 0; i < graphicsPipe->qPipelines; i += 1) {
        graphicsPipe->pipeline[i].pipeline = createGraphicsPipeline(
            builder, 
            graphics->device, 
            builder.renderPassCore[i]->renderPass, 
            graphicsPipe->pipelineLayout, 
            graphics->msaaSamples
        );
        graphicsPipe->pipeline[i].core = builder.renderPassCore[i];
    }

    return graphicsPipe;
}

struct Pipeline *createComputePipelineObj(struct ComputePipelineBuilder builder, struct GraphicsSetup *graphics) {
    struct Pipeline *graphicsPipe = calloc(1, sizeof(struct Pipeline)); 

    *graphicsPipe = (struct Pipeline) {
        .device = graphics->device,
        .pipelineLayout = builder.pipelineLayout,
        .qPipelines = 1,
        .pipeline = malloc(sizeof(struct renderPipeline))
    };

    graphicsPipe->pipeline->pipeline = createComputePipeline(
        graphics->device,
        builder.pipelineLayout,
        builder.computeShader
    );

    return graphicsPipe;
}

void destroyPipelineObj(void *pipePtr) {
    struct Pipeline *pipe = pipePtr;
    vkDeviceWaitIdle(pipe->device);

    for (size_t i = 0; i < pipe->qPipelines; i += 1) {
        vkDestroyPipeline(pipe->device, pipe->pipeline[i].pipeline, NULL);
    }

    free(pipe->pipeline);
    free(pipe);
}
