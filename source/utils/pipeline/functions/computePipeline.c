#include <vulkan/vulkan.h>
#include <cglm/cglm.h>

#include "MY_ASSERT.h"

#define UNUSED_RETVAL(x) if (x) {}

static char *loadFile(const char *filename, int *fileCount) {
    FILE *file = fopen(filename, "rb");

    char *buffor = NULL;

    MY_ASSERT(NULL != file);

    fseek(file, 0, SEEK_END);
    *fileCount = ftell(file);
    buffor = malloc(sizeof(char) * *fileCount);

    fseek(file, 0, SEEK_SET);

    UNUSED_RETVAL(fread(buffor, sizeof(char), *fileCount, file));

    fclose(file);

    return buffor;
}

static VkShaderModule createShaderModule(VkDevice device, const char *filename) {
    VkShaderModule shaderModule = NULL;

    int size = 0;
    char *code = loadFile(filename, &size);

    VkShaderModuleCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = size,
        .pCode = (const uint32_t *)code, // might potentially cause problems
        .pNext = NULL,
        .flags = 0
    };

    MY_ASSERT(VK_SUCCESS == vkCreateShaderModule(device, &createInfo, NULL, &shaderModule));

    free(code);

    return shaderModule;
}

VkPipeline createComputePipeline(VkDevice device, VkPipelineLayout computePipelineLayout, const char *shaderName) {
    VkPipeline computePipeline = NULL;

    VkShaderModule computeModule = createShaderModule(device, shaderName);

    VkPipelineShaderStageCreateInfo computeShaderStageInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = computeModule,
        .pName = "main"
    };

    VkComputePipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .layout = computePipelineLayout,
        .stage = computeShaderStageInfo,
    };

    MY_ASSERT(VK_SUCCESS == vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &computePipeline));

    vkDestroyShaderModule(device, computeModule, NULL);

    return computePipeline;
}
