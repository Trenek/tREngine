#include <string.h>

#include <stb_image.h>
#include <vulkan/vulkan.h>

#include "graphicsSetup.h"
#include "textureFunctions.h"
#include "MY_ASSERT.h"
#include "bufferObj.h"
#include "imageOperations.h"

// This table implements the mapping from 8-bit ascii value to 6-bit
// base64 value and it is used during the base64 decoding
// process. Since not all 8-bit values are used, some of them are
// mapped to -1, meaning that there is no 6-bit value associated with
// that 8-bit value.
//
static int UNBASE64[] = {
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0-11
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 12-23
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 24-35
  -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, // 36-47
  52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -2, // 48-59
  -1,  0, -1, -1, -1,  0,  1,  2,  3,  4,  5,  6, // 60-71
  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, // 72-83
  19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, // 84-95
  -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, // 96-107
  37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, // 108-119
  49, 50, 51, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 120-131
};

static void base64_decode(const char *in, const unsigned long in_len, char *out) {
  uint32_t in_index = 0;
  uint32_t out_index = 0;
  char first, second, third, fourth;

  assert(!(in_len & 0x03)); // input must be even multiple of 4

  while( in_index < in_len ) {
    first = UNBASE64[ (int) in[in_index] ];
    second = UNBASE64[ (int) in[in_index + 1] ];
    third = UNBASE64[ (int) in[in_index + 2] ];
    fourth = UNBASE64[ (int) in[in_index + 3] ];

    // reconstruct first byte
    out[out_index++] = (first << 2) | ((second & 0x30) >> 4);

    // reconstruct second byte
    if (in[in_index + 2] != '=') {
      out[out_index++] = ((second & 0xF) << 4) | ((third & 0x3C) >> 2);
    }

    // reconstruct third byte
    if (in[in_index + 3] != '=') {
      out[out_index++] = ((third & 0x3) << 6) | fourth;
    }

    in_index += 4;
  }

  out[out_index] = 0;
}

struct textureData {
    stbi_uc *pixels;

    int width;
    int height;
    int channels;
};

static struct textureData loadFromFile(struct TextureData texturePath) {
    struct textureData data;
    
    data.pixels = stbi_load(
        texturePath.data,
        &data.width, 
        &data.height, 
        &data.channels, 
        STBI_rgb_alpha
    );

    assert(data.pixels != NULL);

    return data;
}

static struct textureData loadFromMemoryVanila(struct TextureData texturePath) {
    struct textureData data;

    data.pixels = stbi_load_from_memory(
        (unsigned char *)texturePath.data,
        texturePath.qData,
        &data.width,
        &data.height,
        &data.channels,
        STBI_rgb_alpha
    );

    assert(data.pixels != NULL);
    
    return data;
}

static struct textureData loadFromMemoryBase64(struct TextureData texturePath) {
    const char *code = strstr(texturePath.data, ",") + 1;
    unsigned long codeLen = strlen(code);

    int padding = 
        (codeLen >= 1 && code[codeLen - 1] == '=') +
        (codeLen >= 2 && code[codeLen - 2] == '=');

    unsigned long decodedLen = (codeLen / 4) * 3 - padding;

    char decoded[(codeLen * 4) / 4 + 4] = {};
    base64_decode(code, codeLen, decoded);

    return loadFromMemoryVanila((struct TextureData) {
        .data = (char *)decoded,
        .qData = decodedLen
    });
}

static struct textureData loadDefault(struct TextureData) {
    struct textureData data = {
        .width = 1,
        .height = 1,
        .channels = 1,
        .pixels = malloc(sizeof(char) * 4)
    };

    data.pixels[0] = -1;
    data.pixels[1] = -1;
    data.pixels[2] = -1;
    data.pixels[3] = -1;

    return data;
}

struct BufferObj *loadNormalImageToBuffer(struct TextureData texturePath, VkExtent2D *extent, struct GraphicsSetup *graphics) {
    struct textureData (*loadTexture)(struct TextureData) =
        texturePath.data == NULL ?                   loadDefault :
        strncmp(texturePath.data, "data:", 5) == 0 ? loadFromMemoryBase64 :
        texturePath.mode == FROM_MEMORY ?            loadFromMemoryVanila :
                                                     loadFromFile;

    struct textureData data = loadTexture(texturePath);

    VkDeviceSize imageSize = data.width * data.height * 4;

    MY_ASSERT(NULL != data.pixels);

    struct BufferObj *staging = createBufferObj((struct BufferBuilder) {
        .bufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .memoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        .size = imageSize,
        .repetitions = 1
    }, graphics);

    void *dataD;
    vkMapMemory(graphics->device, staging->memory, 0, imageSize, 0, &dataD);
    memcpy(dataD, data.pixels, imageSize);
    vkUnmapMemory(graphics->device, staging->memory);

    stbi_image_free(data.pixels);

    extent->width = data.width;
    extent->height = data.height;

    return staging;
}

struct BufferObj *loadCubeMapImageToBuffer(const char *texturePath[6], struct GraphicsSetup *graphics, VkExtent2D *extent) {
    int width = 0;
    int height = 0;
    int channels = 0;

    stbi_uc *pixels[6] = {};

    for (int i = 0; i < 6; i += 1) {
        pixels[i] = stbi_load(texturePath[i], &width, &height, &channels, STBI_rgb_alpha);
    }
    VkDeviceSize layerSize = width * height * 4;
    VkDeviceSize imageSize = 6 * layerSize;

    for (int i = 0; i < 6; i += 1) {
        MY_ASSERT(NULL != pixels[i]);
    }

    struct BufferObj *staging = createBufferObj((struct BufferBuilder) {
        .bufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .memoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        .size = imageSize,
        .repetitions = 1,
    }, graphics);

    void *data;
    vkMapMemory(graphics->device, staging->memory, 0, imageSize, 0, &data);
    for (int i = 0; i < 6; i += 1) {
        memcpy((char *)data + i * layerSize, pixels[i], layerSize);
    }
    vkUnmapMemory(graphics->device, staging->memory);

    for (int i = 0; i < 6; i += 1) {
        stbi_image_free(pixels[i]);
    }

    extent->width = width;
    extent->height = height;

    return staging;
}
