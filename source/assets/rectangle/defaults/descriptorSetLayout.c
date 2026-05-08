#include "rectangle.h"

#include "descriptorSetLayoutObj.h"

struct descriptorSetLayout *defaultRecDescriptorSetLayout(VkDevice device) {
    return createDescriptorSetLayoutObj(1, (VkDescriptorSetLayoutBinding []) {
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = NULL
            },
        },
        device
    );
}
