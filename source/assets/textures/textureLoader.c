#include <math.h>

#include "graphicsSetup.h"

#include "MY_ASSERT.h"
#include "textureFunctions.h"
#include "commonOperations.h"

#include "descriptorObj.h"
#include "bufferObj.h"
#include "imageObj.h"

#define MAX(x, y) ((x) > (y) ? (x) : (y))

static struct ImageObj *loadCubeMap(struct TextureLoaded texture[6], struct GraphicsSetup *graphics) {
    VkExtent2D extent = {
        .height = texture->extent.height,
        .width = texture->extent.width
    };
    uint32_t mipLevels = 1;

    struct ImageObj *result = createImageObj((struct ImageBuilder) {
        .extent = extent,
        .mipLevels = mipLevels,
        .arrayLayers = 6,
        .format = VK_FORMAT_R8G8B8A8_SRGB,
        .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | 
                 VK_IMAGE_USAGE_TRANSFER_SRC_BIT | 
                 VK_IMAGE_USAGE_SAMPLED_BIT,
        .flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
    }, graphics);

    struct BufferObj *staging = loadImagesToBuffer(6, texture, graphics);

    MY_ASSERT(VK_SUCCESS == vkCreateImageView(graphics->device, &(VkImageViewCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = result->image,
        .viewType = VK_IMAGE_VIEW_TYPE_CUBE,
        .format = VK_FORMAT_R8G8B8A8_SRGB,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = mipLevels,
            .baseArrayLayer = 0,
            .layerCount = 6
        }
    }, NULL, &result->imageView));

    createImageSampler(result, graphics->device, graphics->physicalDevice, (struct SamplerBuilder) {
        .magFilter = VK_FILTER_LINEAR,
        .mipLevels = mipLevels,
        .mipMapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .asinotropyEnable = VK_TRUE
    });

    VkCommandBuffer commandBuffer = beginSingleTimeCommands(graphics->device, graphics->transferCommandPool);
    copyCubeMapTexture(commandBuffer, result->image, mipLevels, staging, extent);
    endSingleTimeCommands(commandBuffer, graphics->device, graphics->transferCommandPool, graphics->transferQueue);

    destroyBufferObj(staging);

    return result;
}

static struct ImageObj *createTexture(struct TextureLoaded texturePath, struct GraphicsSetup *graphics) {
    VkExtent2D extent = {
        .width = texturePath.extent.width,
        .height = texturePath.extent.height,
    };

    uint32_t mipLevels = floor(log2(MAX(extent.width, extent.height))) + 1;

    struct ImageObj *result = createImageObj((struct ImageBuilder) {
        .extent = extent,
        .mipLevels = mipLevels,
        .arrayLayers = 1,
        .format = VK_FORMAT_R8G8B8A8_SRGB,
        .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | 
                 VK_IMAGE_USAGE_TRANSFER_SRC_BIT | 
                 VK_IMAGE_USAGE_SAMPLED_BIT,
    }, graphics);

    MY_ASSERT(VK_SUCCESS == vkCreateImageView(graphics->device, &(VkImageViewCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = result->image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = VK_FORMAT_R8G8B8A8_SRGB,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = mipLevels,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    }, NULL, &result->imageView));

    createImageSampler(result, graphics->device, graphics->physicalDevice, (struct SamplerBuilder) {
        .magFilter = VK_FILTER_LINEAR,
        .mipLevels = mipLevels,
        .mipMapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .asinotropyEnable = VK_TRUE
    });

    return result;
}

static struct ImageObj *createUintTexture(struct TextureLoaded texturePath, struct GraphicsSetup *graphics) {
    VkExtent2D extent = {
        .width = texturePath.extent.width,
        .height = texturePath.extent.height,
    };

    uint32_t mipLevels = floor(log2(MAX(extent.width, extent.height))) + 1;

    struct ImageObj *result = createImageObj((struct ImageBuilder) {
        .extent = extent,
        .mipLevels = mipLevels,
        .arrayLayers = 1,
        .format = VK_FORMAT_R8G8B8A8_UINT,
        .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | 
                 VK_IMAGE_USAGE_TRANSFER_SRC_BIT | 
                 VK_IMAGE_USAGE_SAMPLED_BIT,
    }, graphics);

    MY_ASSERT(VK_SUCCESS == vkCreateImageView(graphics->device, &(VkImageViewCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = result->image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = VK_FORMAT_R8G8B8A8_UINT,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = mipLevels,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    }, NULL, &result->imageView));

    createImageSampler(result, graphics->device, graphics->physicalDevice, (struct SamplerBuilder) {
        .magFilter = VK_FILTER_NEAREST,
        .mipLevels = 0.0f,
        .mipMapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
        .asinotropyEnable = VK_FALSE
    });

    return result;
}

struct Textures *loadCubeMaps(struct GraphicsSetup *graphics, const char *texturePath[6]) {
    struct Textures *texture = calloc(1, sizeof(struct Textures));
    *texture = (struct Textures){
        .device = graphics->device,
        .data = calloc(1, sizeof(struct ImageObj *)),
        .quantity = 1,
        .descriptorLayout = createDescriptorSetLayoutObj(1, (VkDescriptorSetLayoutBinding []) {
            {
                .binding = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImmutableSamplers = NULL,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
            }
        }, graphics->device),
    };

    struct TextureData textureData[6] = {
        { .data = (char *)texturePath[0] },
        { .data = (char *)texturePath[1] },
        { .data = (char *)texturePath[2] },
        { .data = (char *)texturePath[3] },
        { .data = (char *)texturePath[4] },
        { .data = (char *)texturePath[5] },
    };
    struct TextureLoaded loaded[6]; {
        loadTextureFiles(6, textureData, loaded);
    }

    loadCubeMap(loaded, graphics);

    texture->descriptor = createDescriptorSetsObj(graphics, &(struct DescriptorObjBuilder) {
        .layout = texture->descriptorLayout->descriptorSetLayout,
        .qDescriptorPoolSize = 1,
        .descriptorPoolSize = (VkDescriptorPoolSize []) {
            {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = MAX_FRAMES_IN_FLIGHT
            }
        }
    });

    bindImagesToDescriptorSets(texture->descriptor->descriptorSets, graphics->device, (struct ImageBinder) {
        .qImage = 1, 
        .image = texture->data,
        .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    });

    return texture;
}

struct Textures *loadTextures(struct GraphicsSetup *graphics, uint32_t texturesQuantity, struct TextureData texturePath[static texturesQuantity]) {
    struct Textures *texture = calloc(1, sizeof(struct Textures));
    *texture = (struct Textures) {
        .device = graphics->device,
        .data = calloc(texturesQuantity, sizeof(struct ImageObj *)),
        .quantity = texturesQuantity,
        .descriptorLayout = createDescriptorSetLayoutObj(1, (VkDescriptorSetLayoutBinding []) {
            {
                .binding = 0,
                .descriptorCount = texturesQuantity,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImmutableSamplers = NULL,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
            }
        }, graphics->device),
    };

    struct TextureLoaded loaded[texturesQuantity]; {
        loadTextureFiles(texturesQuantity, texturePath, loaded);
    }

    for (uint32_t i = 0; i < texturesQuantity; i += 1) {
        texture->data[i] = createTexture(loaded[i], graphics);
    }

    struct BufferObj *staging = loadImagesToBuffer(texturesQuantity, loaded, graphics);
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(graphics->device, graphics->transferCommandPool);
    VkDeviceSize prev = 0;
    for (uint32_t i = 0; i < texturesQuantity; i += 1) {
        uint32_t mipLevels = floor(log2(MAX(loaded[i].extent.width, loaded[i].extent.height))) + 1;

        copyTextureBufferPixels(commandBuffer, texture->data[i]->image, mipLevels, staging, loaded[i].extent, VK_FILTER_LINEAR, graphics, prev);

        prev += loaded[i].extent.width * loaded[i].extent.height * 4;
    }
    endSingleTimeCommands(commandBuffer, graphics->device, graphics->transferCommandPool, graphics->transferQueue);
    destroyBufferObj(staging);

    texture->descriptor = createDescriptorSetsObj(graphics, &(struct DescriptorObjBuilder) {
        .layout = texture->descriptorLayout->descriptorSetLayout,
        .qDescriptorPoolSize = 1,
        .descriptorPoolSize = (VkDescriptorPoolSize []) {
            {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = texturesQuantity * MAX_FRAMES_IN_FLIGHT
            }
        }
    });

    bindImagesToDescriptorSets(texture->descriptor->descriptorSets, graphics->device, (struct ImageBinder) {
        .qImage = texturesQuantity, 
        .image = texture->data,
        .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    });

    return texture;
}

struct Textures *loadUintTextures(struct GraphicsSetup *graphics, uint32_t texturesQuantity, struct TextureData texturePath[static texturesQuantity]) {
    struct Textures *texture = calloc(1, sizeof(struct Textures));
    *texture = (struct Textures) {
        .device = graphics->device,
        .data = calloc(texturesQuantity, sizeof(struct ImageObj *)),
        .quantity = texturesQuantity,
        .descriptorLayout = createDescriptorSetLayoutObj(1, (VkDescriptorSetLayoutBinding []) {
            {
                .binding = 0,
                .descriptorCount = texturesQuantity,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImmutableSamplers = NULL,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
            }
        }, graphics->device),
    };
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(graphics->device, graphics->transferCommandPool);

    struct TextureLoaded loaded[texturesQuantity]; {
        loadTextureFiles(texturesQuantity, texturePath, loaded);
    }

    for (uint32_t i = 0; i < texturesQuantity; i += 1) {
        texture->data[i] = createUintTexture(loaded[i], graphics);
    }

    struct BufferObj *staging = loadImagesToBuffer(texturesQuantity, loaded, graphics);
    VkDeviceSize prev = 0;
    for (uint32_t i = 0; i < texturesQuantity; i += 1) {
        uint32_t mipLevels = floor(log2(MAX(loaded[i].extent.width, loaded[i].extent.height))) + 1;

        copyTextureBufferPixels(commandBuffer, texture->data[i]->image, mipLevels, staging, loaded[i].extent, VK_FILTER_NEAREST, graphics, prev);

        prev += loaded[i].extent.width * loaded[i].extent.height * 4;
    }

    texture->descriptor = createDescriptorSetsObj(graphics, &(struct DescriptorObjBuilder) {
        .layout = texture->descriptorLayout->descriptorSetLayout,
        .qDescriptorPoolSize = 1,
        .descriptorPoolSize = (VkDescriptorPoolSize []) {
            {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = texturesQuantity * MAX_FRAMES_IN_FLIGHT
            }
        }
    });

    bindImagesToDescriptorSets(texture->descriptor->descriptorSets, graphics->device, (struct ImageBinder) {
        .qImage = texturesQuantity, 
        .image = texture->data,
        .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    });

    endSingleTimeCommands(commandBuffer, graphics->device, graphics->transferCommandPool, graphics->transferQueue);
    destroyBufferObj(staging);

    return texture;
}

void unloadTextures(void *texturePtr) {
    struct Textures *texture = texturePtr;
    for (uint32_t i = 0; i < texture->quantity; i += 1) {
        destroyImageObj(texture->data[i]);
    }

    free(texture->data);

    destroyDescriptorSetLayout(texture->descriptorLayout);
    destroyDescriptorSets(texture->descriptor);

    free(texture);
}
