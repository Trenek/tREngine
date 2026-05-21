#include <malloc.h>

#include "graphicsPipelineLayout.h"
#include "graphicsSetup.h"

#include "MY_ASSERT.h"

struct PipelineLayout *createPipelineLayout(struct PipelineLayoutBuilder builder, struct GraphicsSetup *graphics) {
    struct PipelineLayout *graphicsPipeLayout = calloc(1, sizeof(struct PipelineLayout)); 

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = builder.qDescriptorSetLayout,
        .pSetLayouts = builder.descriptorSetLayout,
        .pushConstantRangeCount = builder.pushConstantRangeCount,
        .pPushConstantRanges = builder.pPushConstantRanges,
    };

    *graphicsPipeLayout = (struct PipelineLayout) {
        .device = graphics->device,
    };

    MY_ASSERT(VK_SUCCESS == vkCreatePipelineLayout(graphics->device, &pipelineLayoutInfo, NULL, &graphicsPipeLayout->pipelineLayout));

#ifndef NDEBUG
    graphics->debugNamer(graphics->device, &(VkDebugUtilsObjectNameInfoEXT) {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        .objectType = VK_OBJECT_TYPE_PIPELINE_LAYOUT,
        .objectHandle = (uint64_t)graphicsPipeLayout->pipelineLayout,
        .pObjectName = builder.debugName,
    });
#endif

    return graphicsPipeLayout;
}

void destroyPipelineLayoutObj(void *pipePtr) {
    struct PipelineLayout *pipeLayout = pipePtr;
    vkDeviceWaitIdle(pipeLayout->device);

    vkDestroyPipelineLayout(pipeLayout->device, pipeLayout->pipelineLayout, NULL);

    free(pipeLayout);
}
