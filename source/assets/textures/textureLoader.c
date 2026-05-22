#include <math.h>

#include "graphicsSetup.h"

#include "myMalloc.h"
#include "textureFunctions.h"
#include "imageOperations.h"
#include "descriptorSetLayoutObj.h"
#include "descriptorObj.h"
#include "bufferObj.h"

#define MAX(x, y) ((x) > (y) ? (x) : (y))

VkImage createCubeMapTexture(VkDeviceMemory *textureImageMemory, uint32_t mipLevels, struct BufferObj *staging, VkExtent2D extent, struct GraphicsSetup *graphics);
VkImage createTextureBufferPixels(VkDeviceMemory *textureImageMemory, uint32_t mipLevels, struct BufferObj *staging, VkExtent2D extent, VkFormat textureFormat, VkFilter textureFilter, struct GraphicsSetup *graphics);

struct BufferObj *loadNormalImageToBuffer(struct TextureData texturePath, VkExtent2D *extent, struct GraphicsSetup *graphics);
struct BufferObj *loadCubeMapImageToBuffer(const char *texturePath[6], struct GraphicsSetup *graphics, VkExtent2D *extent);

static struct Data loadCubeMap(const char *texturePath[6], struct GraphicsSetup *graphics) {
    struct Data result = { 0 };

    VkExtent2D extent;
    struct BufferObj *staging = loadCubeMapImageToBuffer(texturePath, graphics, &extent);
    uint32_t mipLevels = 1;

    result.image = createCubeMapTexture(&result.imageMemory, mipLevels, staging, extent, graphics);
    result.imageView = createImageView(graphics->device, result.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, VK_IMAGE_VIEW_TYPE_CUBE, 6);
    result.sampler = createTextureSampler(graphics->device, graphics->physicalDevice, VK_FILTER_LINEAR, mipLevels, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_TRUE);

    destroyBufferObj(staging);

    return result;
}

static struct Data loadTexture(struct TextureData texturePath, struct GraphicsSetup *graphics) {
    struct Data result = { 0 };
    VkExtent2D extent;
    struct BufferObj *staging = loadNormalImageToBuffer(texturePath, &extent, graphics);

    uint32_t mipLevels = floor(log2(MAX(extent.width, extent.height))) + 1;

    result.image = createTextureBufferPixels(&result.imageMemory, mipLevels, staging, extent, VK_FORMAT_R8G8B8A8_SRGB, VK_FILTER_LINEAR, graphics);
    result.imageView = createImageView(graphics->device, result.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, VK_IMAGE_VIEW_TYPE_2D, 1);
    result.sampler = createTextureSampler(graphics->device, graphics->physicalDevice, VK_FILTER_LINEAR, mipLevels, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_TRUE);

    destroyBufferObj(staging);

    return result;
}

static struct Data loadUintTexture(struct TextureData texturePath, struct GraphicsSetup *graphics) {
    struct Data result = { 0 };
    VkExtent2D extent;
    struct BufferObj *staging = loadNormalImageToBuffer(texturePath, &extent, graphics);

    uint32_t mipLevels = floor(log2(MAX(extent.width, extent.height))) + 1;

    result.image = createTextureBufferPixels(&result.imageMemory, mipLevels, staging, extent, VK_FORMAT_R8G8B8A8_UINT, VK_FILTER_NEAREST, graphics);
    result.imageView = createImageView(graphics->device, result.image, VK_FORMAT_R8G8B8A8_UINT, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, VK_IMAGE_VIEW_TYPE_2D, 1);
    result.sampler = createTextureSampler(graphics->device, graphics->physicalDevice, VK_FILTER_NEAREST, 0.0f, VK_SAMPLER_MIPMAP_MODE_NEAREST, VK_FALSE);

    destroyBufferObj(staging);

    return result;
}

struct Textures *loadCubeMaps(struct GraphicsSetup *graphics, const char *texturePath[6]) {
    struct Textures *texture = calloc(1, sizeof(struct Textures));
    *texture = (struct Textures){
        .device = graphics->device,
        .data = calloc(1, sizeof(struct Data)),
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

    texture->data[0] = loadCubeMap(texturePath, graphics);

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

    bindTextureBuffersToDescriptorSets(texture->descriptor->descriptorSets, graphics->device, 1, texture);

    return texture;
}

struct Textures *loadTextures(struct GraphicsSetup *graphics, uint32_t texturesQuantity, struct TextureData texturePath[static texturesQuantity]) {
    struct Textures *texture = calloc(1, sizeof(struct Textures));
    *texture = (struct Textures) {
        .device = graphics->device,
        .data = calloc(texturesQuantity, sizeof(struct Data)),
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

    for (uint32_t i = 0; i < texturesQuantity; i += 1) {
        texture->data[i] = loadTexture(texturePath[i], graphics);
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

    bindTextureBuffersToDescriptorSets(texture->descriptor->descriptorSets, graphics->device, texturesQuantity, texture);

    return texture;
}

struct Textures *loadUintTextures(struct GraphicsSetup *graphics, uint32_t texturesQuantity, struct TextureData texturePath[static texturesQuantity]) {
    struct Textures *texture = calloc(1, sizeof(struct Textures));
    *texture = (struct Textures) {
        .device = graphics->device,
        .data = calloc(texturesQuantity, sizeof(struct Data)),
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

    for (uint32_t i = 0; i < texturesQuantity; i += 1) {
        texture->data[i] = loadUintTexture(texturePath[i], graphics);
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

    bindTextureBuffersToDescriptorSets(texture->descriptor->descriptorSets, graphics->device, texturesQuantity, texture);

    return texture;
}

void unloadTextures(void *texturePtr) {
    struct Textures *texture = texturePtr;
    for (uint32_t i = 0; i < texture->quantity; i += 1) {
        vkDestroySampler(texture->device, texture->data[i].sampler, NULL);
        vkDestroyImageView(texture->device, texture->data[i].imageView, NULL);

        vkDestroyImage(texture->device, texture->data[i].image, NULL);
        vkFreeMemory(texture->device, texture->data[i].imageMemory, NULL);
    }

    free(texture->data);

    destroyDescriptorSetLayout(texture->descriptorLayout);
    destroyDescriptorSets(texture->descriptor);

    free(texture);
}
