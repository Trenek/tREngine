#include "graphicsSetup.h"

#include "myMalloc.h"
#include "textureFunctions.h"
#include "imageOperations.h"
#include "descriptor.h"

VkImage createCubeMapTexture(VkDeviceMemory *textureImageMemory, uint32_t *mipLevels, const char *texturePath[6], struct GraphicsSetup *graphics);

static struct Data loadCubeMap(const char *texturePath[6], struct GraphicsSetup *graphics) {
    struct Data result = { 0 };

    result.image = createCubeMapTexture(&result.imageMemory, &result.mipLevels, texturePath, graphics);
    result.imageView = createImageView(graphics->device, result.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, result.mipLevels, VK_IMAGE_VIEW_TYPE_CUBE, 6);
    result.sampler = createTextureSampler(graphics->device, graphics->physicalDevice, VK_FILTER_LINEAR, result.mipLevels, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_TRUE);

    return result;
}

static struct Data loadTexture(struct TextureData texturePath, struct GraphicsSetup *graphics) {
    struct Data result = { 0 };

    result.image = createTextureBuffer(&result.imageMemory, &result.mipLevels, texturePath, VK_FORMAT_R8G8B8A8_SRGB, VK_FILTER_LINEAR, graphics);
    result.imageView = createImageView(graphics->device, result.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, result.mipLevels, VK_IMAGE_VIEW_TYPE_2D, 1);
    result.sampler = createTextureSampler(graphics->device, graphics->physicalDevice, VK_FILTER_LINEAR, result.mipLevels, VK_SAMPLER_MIPMAP_MODE_LINEAR, VK_TRUE);

    return result;
}

static struct Data loadUintTexture(struct TextureData texturePath, struct GraphicsSetup *graphics) {
    struct Data result = { 0 };

    result.image = createTextureBuffer(&result.imageMemory, &result.mipLevels, texturePath, VK_FORMAT_R8G8B8A8_UINT, VK_FILTER_NEAREST, graphics);
    result.imageView = createImageView(graphics->device, result.image, VK_FORMAT_R8G8B8A8_UINT, VK_IMAGE_ASPECT_COLOR_BIT, result.mipLevels, VK_IMAGE_VIEW_TYPE_2D, 1);
    result.sampler = createTextureSampler(graphics->device, graphics->physicalDevice, VK_FILTER_NEAREST, 0.0f, VK_SAMPLER_MIPMAP_MODE_NEAREST, VK_FALSE);

    return result;
}

struct Textures *loadCubeMaps(struct GraphicsSetup *graphics, const char *texturePath[6]) {
    struct Textures *texture = calloc(1, sizeof(struct Textures));
    *texture = (struct Textures){
        .device = graphics->device,
        .data = calloc(1, sizeof(struct Data)),
        .quantity = 1,
        .descriptor = {
            .descriptorSetLayout = createTextureDescriptorSetLayout(graphics->device, 1),
            .descriptorPool = createTextureDescriptorPool(graphics->device, 1)
        }
    };
    struct descriptor *desc = &texture->descriptor;

    texture->data[0] = loadCubeMap(texturePath, graphics);

    createDescriptorSets(desc->descriptorSets, graphics->device, desc->descriptorPool, desc->descriptorSetLayout);
    bindTextureBuffersToDescriptorSets(desc->descriptorSets, graphics->device, 1, texture);

    return texture;
}

struct Textures *loadTextures(struct GraphicsSetup *graphics, uint32_t texturesQuantity, struct TextureData texturePath[static texturesQuantity]) {
    struct Textures *texture = calloc(1, sizeof(struct Textures));
    *texture = (struct Textures) {
        .device = graphics->device,
        .data = calloc(texturesQuantity, sizeof(struct Data)),
        .quantity = texturesQuantity,
        .descriptor = {
            .descriptorSetLayout = createTextureDescriptorSetLayout(graphics->device, texturesQuantity),
            .descriptorPool = createTextureDescriptorPool(graphics->device, texturesQuantity)
        }
    };
    struct descriptor *desc = &texture->descriptor;

    for (uint32_t i = 0; i < texturesQuantity; i += 1) {
        texture->data[i] = loadTexture(texturePath[i], graphics);
    }

    createDescriptorSets(desc->descriptorSets, graphics->device, desc->descriptorPool, desc->descriptorSetLayout);
    bindTextureBuffersToDescriptorSets(desc->descriptorSets, graphics->device, texturesQuantity, texture);

    return texture;
}

struct Textures *loadUintTextures(struct GraphicsSetup *graphics, uint32_t texturesQuantity, struct TextureData texturePath[static texturesQuantity]) {
    struct Textures *texture = calloc(1, sizeof(struct Textures));
    *texture = (struct Textures) {
        .device = graphics->device,
        .data = calloc(texturesQuantity, sizeof(struct Data)),
        .quantity = texturesQuantity,
        .descriptor = {
            .descriptorSetLayout = createTextureDescriptorSetLayout(graphics->device, texturesQuantity),
            .descriptorPool = createTextureDescriptorPool(graphics->device, texturesQuantity)
        }
    };
    struct descriptor *desc = &texture->descriptor;

    for (uint32_t i = 0; i < texturesQuantity; i += 1) {
        texture->data[i] = loadUintTexture(texturePath[i], graphics);
    }

    createDescriptorSets(desc->descriptorSets, graphics->device, desc->descriptorPool, desc->descriptorSetLayout);
    bindTextureBuffersToDescriptorSets(desc->descriptorSets, graphics->device, texturesQuantity, texture);

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

    vkDestroyDescriptorSetLayout(texture->device, texture->descriptor.descriptorSetLayout, NULL);
    vkDestroyDescriptorPool(texture->device, texture->descriptor.descriptorPool, NULL);

    free(texture);
}
