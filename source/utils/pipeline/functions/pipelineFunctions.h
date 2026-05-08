#include <vulkan/vulkan_core.h>

struct GraphicsPipelineBuilder;

VkPipeline createGraphicsPipeline(struct GraphicsPipelineBuilder builder, VkDevice device, VkRenderPass renderPass, VkPipelineLayout pipelineLayout, VkSampleCountFlagBits msaaSamples);
VkPipeline createComputePipeline(VkDevice device, VkPipelineLayout computePipelineLayout, const char *shaderName);
