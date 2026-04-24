#include <vulkan/vulkan.h>
#include <string.h>
#include <stddef.h>

#include "renderPassObj.h"

#include "graphicsSetup.h"
#include "definitions.h"

#include "buffer.h"
#include "descriptor.h"
#include "graphicsPipelineObj.h"

#include "bufferOperations.h"

struct renderPassObj *createRenderPassObj(struct renderPassBuilder builder, struct GraphicsSetup *graphics) {
    struct renderPassObj *result = calloc(1, sizeof(struct renderPassObj));
    *result = (struct renderPassObj){
        .device = graphics->device,
        .renderPass = builder.renderPass,
        .data = malloc(sizeof(struct pipelineConnection) * builder.qData),
        .qData = builder.qData,
        .cameraDescriptorPool = createObjectDescriptorPool(graphics->device, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER),
        .updateCameraBuffer = builder.updateCameraBuffer,
        .camera = malloc(builder.cameraSize)
    };

    memcpy(result->camera, builder.camera, builder.cameraSize);
    memcpy(result->data, builder.data, sizeof(struct pipelineConnection) * builder.qData);
    memcpy(result->color, builder.color, sizeof(double) * 4);
    for (size_t i = 0; i < result->qData; i += 1) {
        while (result->renderPass != result->data[i].pipe->pipeline[result->data[i].pipeNum].core) result->data[i].pipeNum += 1;

        result->data[i].entity = malloc(sizeof(struct Entity *) * result->data[i].qEntity);
        memcpy(result->data[i].entity, builder.data[i].entity, sizeof(struct Entity *) * result->data[i].qEntity);
    }
    memcpy(result->coordinates, builder.coordinates, sizeof(double[4]));

    createBuffers(
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
        builder.cameraBufferSize,
        &result->cameraBuffer.buffers, 
        &result->cameraBuffer.buffersMemory, 
        result->cameraBuffer.buffersMapped, 
        graphics->device, 
        graphics->physicalDevice, 
        graphics->surface
    );

    createDescriptorSets(result->cameraDescriptorSet, graphics->device, result->cameraDescriptorPool, builder.cameraDescriptorSetLayout);
    bindObjectBuffersToDescriptorSets(
        result->cameraDescriptorSet, 
        graphics->device, 
        1, 
        (VkBuffer* []) { &result->cameraBuffer.buffers }, 
        (size_t []) { builder.cameraBufferSize }, 
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
    );

    return result;
}

void destroyRenderPassObj(void *renderPassPtr) {
    struct renderPassObj *renderPass = renderPassPtr;
    vkDestroyDescriptorPool(renderPass->device, renderPass->cameraDescriptorPool, NULL);
    destroyBuffer(renderPass->device, renderPass->cameraBuffer.buffers, renderPass->cameraBuffer.buffersMemory);

    for (size_t i = 0; i < renderPass->qData; i += 1) {
        free(renderPass->data[i].entity);
    }
    free(renderPass->data);
    free(renderPass->camera);
    free(renderPass);
}

void destroyRenderPassObjArr(size_t qRenderPass, struct renderPassObj **renderPass) {
    for (size_t i = 0; i < qRenderPass; i += 1) {
        destroyRenderPassObj(renderPass[i]);
    }
}
