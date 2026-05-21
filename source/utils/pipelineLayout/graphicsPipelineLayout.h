#ifndef GRAPHICS_PIPELINE_LAYOUT_OBJ_H
#define GRAPHICS_PIPELINE_LAYOUT_OBJ_H

#include <vulkan/vulkan_core.h>

struct GraphicsSetup;

struct PipelineLayoutBuilder {
    size_t qDescriptorSetLayout;
    VkDescriptorSetLayout *descriptorSetLayout;
    uint32_t pushConstantRangeCount;
    const VkPushConstantRange* pPushConstantRanges;

    const char * const debugName;
};

struct PipelineLayout {
    VkDevice device;

    VkPipelineLayout pipelineLayout;
};
struct PipelineLayout *createPipelineLayout(struct PipelineLayoutBuilder builder, struct GraphicsSetup *graphics);
void destroyPipelineLayoutObj(void *pipePtr);

#endif
