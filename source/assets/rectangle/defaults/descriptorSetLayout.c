#include "rectangle.h"

#include "descriptor.h"

struct descriptorSetLayout *defaultRecDescriptorSetLayout(VkDevice device) {
    return createDescriptorSetLayoutObj(
        createDescriptorSetLayout(device, 1, (VkDescriptorSetLayoutBinding []) {
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = NULL
            },
        }),
        device
    );
}
