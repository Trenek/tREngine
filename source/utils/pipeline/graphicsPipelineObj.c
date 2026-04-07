#include <malloc.h>

#include "graphicsPipelineObj.h"
#include "renderPassCore.h"

#include "pipelineFunctions.h"
#include "graphicsSetup.h"

struct graphicsPipeline *createObjGraphicsPipeline(struct graphicsPipelineBuilder builder, struct GraphicsSetup *graphics) {
    struct graphicsPipeline *graphicsPipe = calloc(1, sizeof(struct graphicsPipeline)); 

    *graphicsPipe = (struct graphicsPipeline) {
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

void destroyObjGraphicsPipeline(void *pipePtr) {
    struct graphicsPipeline *pipe = pipePtr;
    vkDeviceWaitIdle(pipe->device);

    for (size_t i = 0; i < pipe->qPipelines; i += 1) {
        vkDestroyPipeline(pipe->device, pipe->pipeline[i].pipeline, NULL);
    }

    free(pipe->pipeline);
    free(pipe);
}
