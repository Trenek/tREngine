#include <vulkan/vulkan.h>

#include "MY_ASSERT.h"

VkSampler createTextureSampler(VkDevice device, VkPhysicalDevice physicalDevice, VkFilter magFilter, uint32_t mipLevels, VkSamplerMipmapMode mipMapMode, VkBool32 asinotropyEnable) {
    VkSampler textureSampler = NULL;

    VkPhysicalDeviceProperties properties; {
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    }

    VkSamplerCreateInfo samplerInfo = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = magFilter,
        .minFilter = magFilter,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .anisotropyEnable = asinotropyEnable,
        .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .mipmapMode = mipMapMode,
        .mipLodBias = 0.0f,
        .minLod = 0.0f,
        .maxLod = mipLevels
    };

    MY_ASSERT(VK_SUCCESS == vkCreateSampler(device, &samplerInfo, NULL, &textureSampler));

    return textureSampler;
}
