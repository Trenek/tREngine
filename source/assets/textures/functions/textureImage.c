#include <stb_image.h>
#include <vulkan/vulkan.h>

#include "graphicsSetup.h"
#include "textureFunctions.h"
#include "MY_ASSERT.h"
#include "bufferObj.h"
#include "imageOperations.h"

#define MAX(x, y) ((x) > (y) ? (x) : (y))

struct BufferObj *loadNormalImageToBuffer(struct TextureData texturePath, VkExtent2D *extent, struct GraphicsSetup *graphics);
struct BufferObj *loadCubeMapImageToBuffer(uint32_t *mipLevels, const char *texturePath[6], struct GraphicsSetup *graphics, VkExtent2D *extent);

// transfer command and queue
VkImage createTextureBufferPixels(VkDeviceMemory *textureImageMemory, uint32_t mipLevels, struct BufferObj *staging, VkExtent2D extent, VkFormat textureFormat, VkFilter textureFilter, struct GraphicsSetup *graphics) {
    VkImage textureImage = NULL;

    // TODO - i guess cleanup and better abstractions simular to BufferObj?
    MY_ASSERT(VK_SUCCESS == vkCreateImage(graphics->device, &(VkImageCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .extent = {
            .width = extent.width,
            .height = extent.height,
            .depth = 1
        },
        .mipLevels = mipLevels,
        .arrayLayers = 1,
        .format = textureFormat,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | 
                 VK_IMAGE_USAGE_TRANSFER_SRC_BIT | 
                 VK_IMAGE_USAGE_SAMPLED_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .flags = 0
    }, NULL, &textureImage));
    *textureImageMemory = createImageMemory(graphics->device, graphics->physicalDevice, textureImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    transitionImageLayout2(&(VkImageMemoryBarrier2) {
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
    }, graphics->device, graphics->transferCommandPool, graphics->transferQueue);

    copyBufferToImage(staging->buffer, textureImage, extent, graphics->device, graphics->transferCommandPool, graphics->transferQueue, 1);

    generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_SRGB, extent.width, extent.height, mipLevels, textureFilter, graphics->device, graphics->physicalDevice, graphics->transferCommandPool, graphics->transferQueue);

    return textureImage;
}

VkImage createCubeMapTexture(VkDeviceMemory *textureImageMemory, uint32_t mipLevels, struct BufferObj *staging, VkExtent2D extent, struct GraphicsSetup *graphics) {
    VkImage textureImage = NULL;

    MY_ASSERT(VK_SUCCESS == vkCreateImage(graphics->device, &(VkImageCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .extent = {
            .width = extent.width,
            .height = extent.height,
            .depth = 1
        },
        .mipLevels = mipLevels,
        .arrayLayers = 6,
        .format = VK_FORMAT_R8G8B8A8_SRGB,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | 
                 VK_IMAGE_USAGE_TRANSFER_SRC_BIT | 
                 VK_IMAGE_USAGE_SAMPLED_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
    }, NULL, &textureImage));
    *textureImageMemory = createImageMemory(graphics->device, graphics->physicalDevice, textureImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels, graphics->device, graphics->transferCommandPool, graphics->transferQueue, 6);
    copyBufferToImage(staging->buffer, textureImage, extent, graphics->device, graphics->transferCommandPool, graphics->transferQueue, 6);
    transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels, graphics->device, graphics->transferCommandPool, graphics->transferQueue, 6);

    return textureImage;
}
