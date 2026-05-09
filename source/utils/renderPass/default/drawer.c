#include <vulkan/vulkan.h>

#include "renderPassObj.h"
#include "entity.h"

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
    int one = renderPass->cameraBuffer.range == 0;

    for (uint32_t i = 0; i < renderPass->qData; i += 1) {
        int two = one + (renderPass->data[i].texture == 0);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderPass->data[i].pipeline);

        if (renderPass->data[i].texture) {
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderPass->data[i].pipelineLayout, 1 - one, 1, &renderPass->data[i].texture->descriptorSets[currentFrame], 0, NULL);
        }

        for (uint32_t j = 0; j < renderPass->data[i].qEntity; j += 1) {
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderPass->data[i].pipelineLayout, 2 - two, 1, &renderPass->data[i].entitySet[j][currentFrame], 0, NULL);
            drawEntity(commandBuffer, renderPass->data[i].qDrawData[j], renderPass->data[i].drawData[j], renderPass->data[i].pipelineLayout);
        }
    }
}
