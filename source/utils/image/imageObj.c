#include <stdlib.h>

#include "graphicsSetup.h"
#include "commonOperations.h"
#include "imageObj.h"

#include "MY_ASSERT.h"

void transitionImageLayout(VkImage image, [[maybe_unused]] VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, VkDevice device, VkCommandPool commandPool, VkQueue queue, uint32_t layerCount);

struct ImageObj *createImageObj(struct ImageBuilder builder, struct GraphicsSetup *graphics) {
    struct ImageObj *result = malloc(sizeof(struct ImageObj));

    result->device = graphics->device;
    vkCreateImage(graphics->device, &(VkImageCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .extent = {
            .width = builder.extent.width,
            .height = builder.extent.height,
            .depth = 1
        },
        .mipLevels = 1,
        .arrayLayers = 1,
        .format = VK_FORMAT_R8G8B8A8_UNORM,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .usage = VK_IMAGE_USAGE_STORAGE_BIT |
                 VK_IMAGE_USAGE_SAMPLED_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .samples = VK_SAMPLE_COUNT_1_BIT
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

    vkCreateSampler(graphics->device, &(VkSamplerCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .anisotropyEnable = VK_FALSE,
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
        .compareEnable = VK_FALSE,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR
    }, NULL, &result->sampler);

    return result;
}

void destroyImageObj(void *imagePtr) {
    struct ImageObj *image = imagePtr;

    vkDestroySampler(image->device, image->sampler, NULL);
    vkDestroyImageView(image->device, image->imageView, NULL);

    vkDestroyImage(image->device, image->image, NULL);
    vkFreeMemory(image->device, image->memory, NULL);

    free(image);
}
