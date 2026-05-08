#include <stdio.h>
#include <string.h>

#include "engineCore.h"

#include "renderPassObj.h"
#include "entity.h"
#include "model.h"

#include "descriptorObj.h"

#include "MY_ASSERT.h"

static void bindDescriptorSets(VkCommandBuffer commandBuffer, uint32_t currentFrame, struct renderPassObj *renderPass, size_t j, size_t k) {
    size_t qSet = renderPass->data[j].texture == NULL ? 2 : 3;
    VkDescriptorSet set[qSet];

    if (renderPass->data[j].texture) {
        set[0] = renderPass->data[j].entitySet[k][currentFrame];
        set[1] = renderPass->data[j].texture->descriptorSets[currentFrame];
        set[2] = renderPass->cameraDescriptorSet[currentFrame];
    }
    else {
        set[0] = renderPass->data[j].entitySet[k][currentFrame];
        set[1] = renderPass->cameraDescriptorSet[currentFrame];
    }

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderPass->data[j].pipelineLayout, 0, qSet, set, 0, NULL);
}

static void drawEntity(VkCommandBuffer commandBuffer, size_t qDraw, struct DrawCall *draw, VkPipelineLayout pipelineLayout) {
    for (uint32_t i = 0; i < qDraw; i += 1) {
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &draw[i].vertexBuffer, (VkDeviceSize[]){ 0 });
        vkCmdBindIndexBuffer(commandBuffer, draw[i].indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        if (0 != draw->pushConstantsSize) {
            vkCmdPushConstants(commandBuffer, pipelineLayout, draw[i].pushConstantsStage, 0, draw[i].pushConstantsSize, draw[i].pushConstans);
        }

        if (NULL != draw[i].indexBuffer) {
            vkCmdDrawIndexed(commandBuffer, draw[i].indicesQuantity, draw[i].instanceCount, 0, 0, 0);
        }
        else {
            vkCmdDraw(commandBuffer, draw[i].verticesQuantity, draw[i].instanceCount, 0, 0);
        }
    }
}

void drawRenderPass(VkCommandBuffer commandBuffer, uint32_t currentFrame, struct renderPassObj *renderPass) {
    for (uint32_t i = 0; i < renderPass->qData; i += 1) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderPass->data[i].pipeline);

        for (uint32_t j = 0; j < renderPass->data[i].qEntity; j += 1) {
            bindDescriptorSets(commandBuffer, currentFrame, renderPass, i, j);
            drawEntity(commandBuffer, renderPass->data[i].qDrawData[j], renderPass->data[i].drawData[j], renderPass->data[i].pipelineLayout);
        }
    }
}
