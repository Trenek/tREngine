#include "engineCore.h"

#include "renderPassObj.h"
#include "entity.h"
#include "model.h"

#include "descriptorObj.h"

#include "MY_ASSERT.h"

void drawRenderPassComp(VkCommandBuffer commandBuffer, uint32_t currentFrame, struct renderPassObj *renderPass) {
    struct DrawCall2 **draw = (void *)renderPass->data->drawData;

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderPass->data->pipeline);

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &draw[0]->vertexBuffer[currentFrame], (VkDeviceSize []) { 0 });

    vkCmdDraw(commandBuffer, draw[0]->verticesQuantity, 1, 0, 0);
}
