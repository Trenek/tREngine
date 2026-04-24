#include <vulkan/vulkan_core.h>

#include "descriptor.h"

struct descriptorSetLayout *defaultCameraDescriptorSetLayout(VkDevice device) {
    return createDescriptorSetLayoutObj(
        createDescriptorSetLayout(device, 1, (VkDescriptorSetLayoutBinding []){
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = NULL
            }
        }),
        device
    );
}
