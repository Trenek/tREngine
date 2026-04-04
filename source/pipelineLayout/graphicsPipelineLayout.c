#include <malloc.h>

#include "graphicsPipelineLayout.h"

#include "graphicsSetup.h"
#include "pushConstantsBuffer.h"

#include "MY_ASSERT.h"

struct graphicsPipelineLayout *createGraphicPipelineLayout(struct graphicsPipelineLayoutBuilder builder, struct GraphicsSetup *graphics) {
    struct graphicsPipelineLayout *graphicsPipeLayout = calloc(1, sizeof(struct graphicsPipelineLayout)); 

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = builder.qDescriptorSetLayout,
        .pSetLayouts = builder.descriptorSetLayout,
        .pushConstantRangeCount = builder.pushConstantRangeCount,
        .pPushConstantRanges = builder.pPushConstantRanges
    };

    *graphicsPipeLayout = (struct graphicsPipelineLayout) {
        .device = graphics->device,
    };

    MY_ASSERT(VK_SUCCESS == vkCreatePipelineLayout(graphics->device, &pipelineLayoutInfo, NULL, &graphicsPipeLayout->pipelineLayout));

    return graphicsPipeLayout;
}

void destroyObjGraphicsPipelineLayout(void *pipePtr) {
    struct graphicsPipelineLayout *pipeLayout = pipePtr;
    vkDeviceWaitIdle(pipeLayout->device);

    vkDestroyPipelineLayout(pipeLayout->device, pipeLayout->pipelineLayout, NULL);

    free(pipeLayout);
}
