#include <vulkan/vulkan_core.h>

struct graphicsPipelineBuilder;

VkPipelineLayout createPipelineLayout(VkDevice device, VkPipelineLayoutCreateInfo pipelineLayoutInfo);
VkPipeline createGraphicsPipeline(struct graphicsPipelineBuilder builder, VkDevice device, VkRenderPass renderPass, VkPipelineLayout pipelineLayout, VkSampleCountFlagBits msaaSamples);
