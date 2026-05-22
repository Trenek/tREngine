#include <stb_image.h>
#include <vulkan/vulkan.h>

#include "graphicsSetup.h"
#include "bufferObj.h"
#include "imageOperations.h"
#include "imageObj.h"

void copyTextureBufferPixels(VkCommandBuffer commandBuffer, VkImage textureImage, uint32_t mipLevels, struct BufferObj *staging, VkExtent3D extent, VkFilter textureFilter, struct GraphicsSetup *graphics, VkDeviceSize prev) {
    VkExtent2D extent2D = {
        .width = extent.width,
        .height = extent.height
    };
    transitionImageLayout2(commandBuffer, &(VkImageMemoryBarrier2) {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,

        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,

        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,

        .srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT,

        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .image = textureImage,

        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = mipLevels,
            .baseArrayLayer = 0,
            .layerCount = 1
        },
    });

    copyBufferToImage(commandBuffer, staging->buffer, textureImage, extent2D, 1, prev);

    generateMipmaps(commandBuffer, textureImage, (struct MipmapsBuilder) {
        .filter = textureFilter,
        .mipLevels = mipLevels,
        .format = VK_FORMAT_R8G8B8A8_SRGB,
        .extent = extent2D,
    }, graphics->physicalDevice);
}

void copyCubeMapTexture(VkCommandBuffer commandBuffer, VkImage textureImage, uint32_t mipLevels, struct BufferObj *staging, VkExtent2D extent) {
    transitionImageLayout2(commandBuffer, &(VkImageMemoryBarrier2) {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,

        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,

        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,

        .srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT,

        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .image = textureImage,

        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = mipLevels,
            .baseArrayLayer = 0,
            .layerCount = 6
        },
    });

    copyBufferToImage(commandBuffer, staging->buffer, textureImage, extent, 6, 0);

    transitionImageLayout2(commandBuffer, &(VkImageMemoryBarrier2) {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,

        .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,

        .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,

        .srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,

        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,

        .image = textureImage,

        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = mipLevels,
            .baseArrayLayer = 0,
            .layerCount = 6
        },
    });
}
