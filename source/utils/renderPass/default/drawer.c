#include <vulkan/vulkan.h>

#include "renderPassObj.h"
#include "entity.h"

static void drawEntity(VkCommandBuffer commandBuffer, size_t qDraw, struct DrawCall *draw, VkPipelineLayout pipelineLayout) {
    for (uint32_t i = 0; i < qDraw; i += 1) {
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &draw[i].vertexBuffer, (VkDeviceSize[]){ 0 });
        vkCmdBindIndexBuffer(commandBuffer, draw[i].indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        if (0 != draw[i].pushConstantsSize) {
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
    VkDescriptorSet descriptors[2];
    size_t q1 = 0;

    if (renderPass->cameraDescriptor != NULL) {
        descriptors[0] = renderPass->cameraDescriptor->descriptorSets[currentFrame];
        q1 += 1;
    }

    for (size_t i = 0; i < renderPass->qData; i += 1) {
        size_t q2 = q1;

        if (NULL != renderPass->data[i].texture) {
            descriptors[q1] = renderPass->data[i].texture[currentFrame];
            
            q2 = q1 + 1;
        }

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderPass->data[i].pipeline);

        // TODO: use vkCmdBindDescriptorSets2 in the future as it's nicer
        if (0 != q2) {
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderPass->data[i].pipelineLayout, 0, q2, descriptors, 0, NULL);
        }

        for (size_t j = 0; j < renderPass->data[i].qEntity; j += 1) {
            if (renderPass->data[i].entitySet[j]) {
                vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderPass->data[i].pipelineLayout, q2, 1, &renderPass->data[i].entitySet[j][currentFrame], 0, NULL);
            }
            drawEntity(commandBuffer, renderPass->data[i].qDrawData[j], renderPass->data[i].drawData[j], renderPass->data[i].pipelineLayout);
        }
    }
}
