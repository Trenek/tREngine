#include <vulkan/vulkan.h>
#include <string.h>
#include <stddef.h>

#include "renderPassObj.h"

#include "graphicsSetup.h"
#include "definitions.h"

#include "buffer.h"
#include "descriptor.h"
#include "cameraBufferObject.h"
#include "graphicsPipelineObj.h"

struct renderPassObj *createRenderPassObj(struct renderPassBuilder builder, struct GraphicsSetup *graphics) {
    struct renderPassObj *result = calloc(1, sizeof(struct renderPassObj));
    *result = (struct renderPassObj){
        .device = graphics->device,
        .renderPass = builder.renderPass,
        .data = malloc(sizeof(struct pipelineConnection) * builder.qData),
        .qData = builder.qData,
        .cameraDescriptorPool = createCameraDescriptorPool(graphics->device),
        .cameraDescriptorSetLayout = createCameraDescriptorSetLayout(graphics->device),
        .updateCameraBuffer = builder.updateCameraBuffer,
        .camera = builder.camera
    };
    glm_vec3_normalize(result->camera.direction);

    memcpy(result->data, builder.data, sizeof(struct pipelineConnection) * builder.qData);
    memcpy(result->color, builder.color, sizeof(double) * 4);
    for (size_t i = 0; i < result->qData; i += 1) {
        while (result->renderPass != result->data[i].pipe->pipeline[result->data[i].pipeNum].core) result->data[i].pipeNum += 1;

        result->data[i].entity = malloc(sizeof(struct Entity *) * result->data[i].qEntity);
        memcpy(result->data[i].entity, builder.data[i].entity, sizeof(struct Entity *) * result->data[i].qEntity);
    }
    memcpy(result->coordinates, builder.coordinates, sizeof(double[4]));

    createBuffers(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(struct CameraBuffer), result->cameraBuffer, result->cameraBufferMemory, result->cameraBufferMapped, graphics->device, graphics->physicalDevice, graphics->surface);

    createDescriptorSets(result->cameraDescriptorSet, graphics->device, result->cameraDescriptorPool, result->cameraDescriptorSetLayout);
    bindCameraBuffersToDescriptorSets(result->cameraDescriptorSet, graphics->device, result->cameraBuffer);

    return result;
}

void destroyRenderPassObj(void *renderPassPtr) {
    struct renderPassObj *renderPass = renderPassPtr;
    vkDestroyDescriptorPool(renderPass->device, renderPass->cameraDescriptorPool, NULL);
    vkDestroyDescriptorSetLayout(renderPass->device, renderPass->cameraDescriptorSetLayout, NULL);
    destroyBuffers(renderPass->device, renderPass->cameraBuffer, renderPass->cameraBufferMemory);

    for (size_t i = 0; i < renderPass->qData; i += 1) {
        free(renderPass->data[i].entity);
    }
    free(renderPass->data);
    free(renderPass);
}

void destroyRenderPassObjArr(size_t qRenderPass, struct renderPassObj **renderPass) {
    for (size_t i = 0; i < qRenderPass; i += 1) {
        destroyRenderPassObj(renderPass[i]);
    }
}
