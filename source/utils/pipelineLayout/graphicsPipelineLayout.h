#ifndef GRAPHICS_PIPELINE_LAYOUT_OBJ_H
#define GRAPHICS_PIPELINE_LAYOUT_OBJ_H

#include <vulkan/vulkan_core.h>

struct GraphicsSetup;

struct graphicsPipelineLayoutBuilder {
    size_t qDescriptorSetLayout;
    VkDescriptorSetLayout *descriptorSetLayout;
    uint32_t pushConstantRangeCount;
    const VkPushConstantRange* pPushConstantRanges;
};

struct graphicsPipelineLayout {
    VkDevice device;

    VkPipelineLayout pipelineLayout;
};
struct graphicsPipelineLayout *createGraphicPipelineLayout(struct graphicsPipelineLayoutBuilder builder, struct GraphicsSetup *graphics);
void destroyObjGraphicsPipelineLayout(void *pipePtr);

#endif
