#include <vulkan/vulkan_core.h>

struct graphicsPipelineBuilder;

VkPipeline createGraphicsPipeline(struct graphicsPipelineBuilder builder, VkDevice device, VkRenderPass renderPass, VkPipelineLayout pipelineLayout, VkSampleCountFlagBits msaaSamples);
