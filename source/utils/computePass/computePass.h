#include <vulkan/vulkan.h>

struct ComputePass {
    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;

    size_t groupCountX;
    VkDescriptorSet *descriptor;
};
