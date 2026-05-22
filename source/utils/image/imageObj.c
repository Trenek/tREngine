#include <stdlib.h>

#include "graphicsSetup.h"
#include "commonOperations.h"
#include "imageOperations.h"
#include "definitions.h"
#include "imageObj.h"

#include "MY_ASSERT.h"

struct ImageObj *createImageObj(struct ImageBuilder builder, struct GraphicsSetup *graphics) {
    struct ImageObj *result = calloc(1, sizeof(struct ImageObj));

    result->device = graphics->device;
    vkCreateImage(graphics->device, &(VkImageCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .extent = {
            .width = builder.extent.width,
            .height = builder.extent.height,
            .depth = 1
        },
        .mipLevels = builder.mipLevels,
        .arrayLayers = builder.arrayLayers,
        .format = builder.format,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .usage = builder.usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .flags = builder.flags,
    }, NULL, &result->image);

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(graphics->device, result->image, &memRequirements);
    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = findMemoryType(graphics->physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    };

    MY_ASSERT(VK_SUCCESS == vkAllocateMemory(graphics->device, &allocInfo, NULL, &result->memory));
    vkBindImageMemory(graphics->device, result->image, result->memory, 0);

    return result;
}

void createImageSampler(struct ImageObj *image, VkDevice device, VkPhysicalDevice physicalDevice, struct SamplerBuilder builder) {
    VkPhysicalDeviceProperties properties; {
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    }

    VkSamplerCreateInfo samplerInfo = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = builder.magFilter,
        .minFilter = builder.magFilter,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .anisotropyEnable = builder.asinotropyEnable,
        .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .mipmapMode = builder.mipMapMode,
        .mipLodBias = 0.0f,
        .minLod = 0.0f,
        .maxLod = builder.mipLevels
    };

    MY_ASSERT(VK_SUCCESS == vkCreateSampler(device, &samplerInfo, NULL, &image->sampler));
}

void completeImageObj(struct ImageObj *result, struct GraphicsSetup *graphics) {
    transitionImageLayout(result->image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, 1, graphics->device, graphics->transferCommandPool, graphics->transferQueue, 1);

    vkCreateImageView(graphics->device, &(VkImageViewCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = result->image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = VK_FORMAT_R8G8B8A8_UNORM,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    }, NULL, &result->imageView);
}

void destroyImageObj(void *imagePtr) {
    struct ImageObj *image = imagePtr;

    vkDestroySampler(image->device, image->sampler, NULL);
    vkDestroyImageView(image->device, image->imageView, NULL);

    vkDestroyImage(image->device, image->image, NULL);
    vkFreeMemory(image->device, image->memory, NULL);

    free(image);
}

struct MipmapsBuilder {
    VkFormat format;
    VkExtent2D extent;
    uint32_t mipLevels;
    VkFilter filter;
};

void generateMipmaps(VkCommandBuffer commandBuffer, VkImage image, struct MipmapsBuilder builder, VkPhysicalDevice physicalDevice) {
    int32_t mipWidth = builder.extent.width;
    int32_t mipHeight = builder.extent.height;

    VkFormatProperties formatProperties; {
        vkGetPhysicalDeviceFormatProperties(physicalDevice, builder.format, &formatProperties);
    }

    MY_ASSERT(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT);

    for (uint32_t i = 1; i < builder.mipLevels; i += 1) {
        vkCmdPipelineBarrier2(commandBuffer, &(VkDependencyInfo) {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &(VkImageMemoryBarrier2) {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                .image = image,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

                .srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT,
                .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                    .levelCount = 1,
                    .baseMipLevel = i - 1
                }
            }
        });

        VkImageBlit2 blit = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
            .srcOffsets = {
                [0] = { 0, 0, 0 },
                [1] = { mipWidth, mipHeight, 1 }
            },
            .srcSubresource = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = i - 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            },
            .dstOffsets = {
                [0] = { 0, 0, 0 },
                [1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 }
            },
            .dstSubresource = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = i,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        vkCmdBlitImage2(commandBuffer, &(VkBlitImageInfo2) {
            .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
            .srcImage = image,
            .dstImage = image,
            .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .regionCount = 1,
            .pRegions = &blit,
            .filter = builder.filter,
        });

        vkCmdPipelineBarrier2(commandBuffer, &(VkDependencyInfo) {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &(VkImageMemoryBarrier2) {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                .image = image,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

                .srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                .srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
                .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,

                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                    .levelCount = 1,
                    .baseMipLevel = i - 1
                }
            }
        });

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    vkCmdPipelineBarrier2(commandBuffer, &(VkDependencyInfo) {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &(VkImageMemoryBarrier2) {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .image = image,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

            .srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,

            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseArrayLayer = 0,
                .layerCount = 1,
                .levelCount = 1,
                .baseMipLevel = builder.mipLevels - 1
            }
        }
    });
}

void bindImagesToDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, struct ImageBinder binder) {
    VkDescriptorImageInfo imageInfoArray[binder.qImage];
    VkWriteDescriptorSet descriptorWrites[MAX_FRAMES_IN_FLIGHT];

    for (uint32_t i = 0; i < binder.qImage; i += 1) {
        imageInfoArray[i] = (VkDescriptorImageInfo) {
            .imageLayout = binder.layout,
            .imageView = binder.image[i]->imageView,
            .sampler = binder.image[i]->sampler
        };
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i += 1) {
        descriptorWrites[i] = (VkWriteDescriptorSet){
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSets[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorType = binder.descriptorType,
            .descriptorCount = binder.qImage,
            .pImageInfo = imageInfoArray,
            .pTexelBufferView = NULL
        };
    }

    vkUpdateDescriptorSets(device, MAX_FRAMES_IN_FLIGHT, descriptorWrites, 0, NULL);
}
