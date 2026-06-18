#include <vulkan/vulkan_core.h>

#include "descriptorSetLayoutObj.h"

struct DescriptorSetLayout *defaultCameraDescriptorSetLayout(VkDevice device) {
    return createDescriptorSetLayoutObj(1, (VkDescriptorSetLayoutBinding []){
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
                .pImmutableSamplers = NULL
            }
        },
        device
    );
}
