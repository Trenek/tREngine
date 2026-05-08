#include <vulkan/vulkan.h>
#include <string.h>
#include <stddef.h>
#include <malloc.h>

#include "renderPassObj.h"

#include "graphicsSetup.h"
#include "definitions.h"

#include "buffer.h"
#include "descriptor.h"
#include "graphicsPipelineObj.h"

#include "bufferOperations.h"
#include "entity.h"

static struct BuffersToUpdate *consolidateBuffers(struct renderPassBuilder builder, size_t qBuffersToUpdate) {
    struct BuffersToUpdate *result = malloc(sizeof(struct BuffersToUpdate) * qBuffersToUpdate);
    
    for (size_t i = 0, k0 = 0; i < builder.qData; i += 1) {
        for (size_t j = 0; j < builder.data[i].qEntity; j += 1) {
            for (size_t k = 0; k < builder.data[i].entity[j]->qBuff; k += 1) {
                if (builder.data[i].entity[j]->buffer[k]) {
                    result[k0] = (struct BuffersToUpdate) {
                        .buffer = builder.data[i].entity[j]->buffer[k],
                        .range = builder.data[i].entity[j]->range[k],
                        .mapp = *builder.data[i].entity[j]->mapp[k],
                    };
                    
                    k0 += 1;
                }
            }
        } 
    }

    return result;
}

struct renderPassObj *createRenderPassObj(struct renderPassBuilder builder, struct GraphicsSetup *graphics) {
    struct renderPassObj *result = calloc(1, sizeof(struct renderPassObj));
    *result = (struct renderPassObj){
        .device = graphics->device,
        .renderPass = builder.renderPass,
        .data = malloc(sizeof(struct pipelineConnection) * builder.qData),
        .qData = builder.qData,
        .cameraDescriptorPool = createDescriptorPool(graphics->device, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER),
        .updateCameraBuffer = builder.camera.updateBuffer,
        .camera = malloc(builder.camera.size),
        .drawRenderPass = builder.drawRenderPass,
    };

    memcpy(result->camera, builder.camera.mapped, builder.camera.size);
    memcpy(result->data, builder.data, sizeof(struct pipelineConnection) * builder.qData);
    memcpy(result->color, builder.color, sizeof(double) * 4);

    for (size_t i = 0; i < builder.qData; i += 1) {
        int pipeNum = 0;
        result->data[i].texture = builder.data[i].texture;
        result->data[i].qEntity = builder.data[i].qEntity;

        while (result->renderPass != builder.data[i].pipe->pipeline[pipeNum].core) pipeNum += 1;
        result->data[i].pipeline = builder.data[i].pipe->pipeline[pipeNum].pipeline;
        result->data[i].pipelineLayout = builder.data[i].pipe->pipelineLayout;

        result->data[i].drawData = malloc(sizeof(void *) * result->data[i].qEntity);
        result->data[i].qDrawData = malloc(sizeof(size_t) * result->data[i].qEntity);
        result->data[i].entitySet = malloc(sizeof(VkDescriptorSet) * result->data[i].qEntity);

        for (size_t j = 0; j < builder.data[i].qEntity; j += 1) {
            result->data[i].qDrawData[j] = builder.data[i].entity[j]->drawCallQuantity;
            result->data[i].drawData[j] = builder.data[i].entity[j]->drawCall;
            result->data[i].entitySet[j] = builder.data[i].entity[j]->object.descriptorSets;
            for (size_t k = 0; k < builder.data[i].entity[j]->qBuff; k += 1) {
                if (builder.data[i].entity[j]->buffer[k]) {
                    result->qBuffersToUpdate += 1;
                }
            }
        } 
    }

    result->buffersToUpdate = consolidateBuffers(builder, result->qBuffersToUpdate);
    memcpy(result->coordinates, builder.coordinates, sizeof(double[4]));

    createBuffers(
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
        builder.camera.bufferSize,
        &result->cameraBuffer.buffers, 
        &result->cameraBuffer.buffersMemory, 
        result->cameraBuffer.buffersMapped, 
        graphics->device, 
        graphics->physicalDevice, 
        graphics->surface
    );

    createDescriptorSets(result->cameraDescriptorSet, graphics->device, result->cameraDescriptorPool, builder.cameraDescriptorSetLayout);
    bindBuffersToDescriptorSets(
        result->cameraDescriptorSet, 
        graphics->device, 
        1, 
        (VkBuffer* []) { &result->cameraBuffer.buffers }, 
        (size_t []) { builder.camera.bufferSize }, 
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
    );

    return result;
}

void destroyRenderPassObj(void *renderPassPtr) {
    struct renderPassObj *renderPass = renderPassPtr;
    vkDestroyDescriptorPool(renderPass->device, renderPass->cameraDescriptorPool, NULL);
    destroyBuffer(renderPass->device, renderPass->cameraBuffer.buffers, renderPass->cameraBuffer.buffersMemory);

    for (size_t i = 0; i < renderPass->qData; i += 1) {
        free(renderPass->data[i].drawData);
        free(renderPass->data[i].entitySet);
    }

    free(renderPass->buffersToUpdate);
    free(renderPass->data);
    free(renderPass->camera);
    free(renderPass);
}

void destroyRenderPassObjArr(size_t qRenderPass, struct renderPassObj **renderPass) {
    for (size_t i = 0; i < qRenderPass; i += 1) {
        destroyRenderPassObj(renderPass[i]);
    }
}
