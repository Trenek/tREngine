#include <string.h>

#include <stb_image.h>
#include <vulkan/vulkan.h>

#include "graphicsSetup.h"
#include "textureFunctions.h"
#include "MY_ASSERT.h"
#include "bufferObj.h"

static int UNBASE64[] = {
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
  52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -2,
  -1,  0, -1, -1, -1,  0,  1,  2,  3,  4,  5,  6,
   7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
  19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
  -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
  37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
  49, 50, 51, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

static void base64_decode(const int8_t *in, const unsigned long in_len, char *out) {
    uint32_t in_index = 0;
    uint32_t out_index = 0;

    char first = 0;
    char second = 0;
    char third = 0;
    char fourth = 0;

    assert(!(in_len & 0x03));

    while (in_index < in_len) {
        first  = UNBASE64[in[in_index + 0]];
        second = UNBASE64[in[in_index + 1]];
        third  = UNBASE64[in[in_index + 2]];
        fourth = UNBASE64[in[in_index + 3]];

        out[out_index++] = (first << 2) | ((second & 0x30) >> 4);

        if (in[in_index + 2] != '=') {
            out[out_index++] = ((second & 0xF) << 4) | ((third & 0x3C) >> 2);
        }

        if (in[in_index + 3] != '=') {
            out[out_index++] = ((third & 0x3) << 6) | fourth;
        }

        in_index += 4;
    }

    out[out_index] = 0;
}

static struct TextureLoaded loadFromFile(struct TextureData texturePath) {
    int width;
    int height;
    struct TextureLoaded data;
    
    data.pixels = stbi_load(
        texturePath.data,
        &width, 
        &height, 
        &data.channels, 
        STBI_rgb_alpha
    );

    data.extent = (VkExtent3D){
        .width = width,
        .height = height,
        .depth = 1
    };
    assert(data.pixels != NULL);

    return data;
}

static struct TextureLoaded loadFromMemoryVanila(struct TextureData texturePath) {
    struct TextureLoaded data;
    int width;
    int height;

    data.pixels = stbi_load_from_memory(
        (unsigned char *)texturePath.data,
        texturePath.qData,
        &width,
        &height,
        &data.channels,
        STBI_rgb_alpha
    );

    data.extent = (VkExtent3D){
        .width = width,
        .height = height,
        .depth = 1
    };
    assert(data.pixels != NULL);
    
    return data;
}

static struct TextureLoaded loadFromMemoryBase64(struct TextureData texturePath) {
    union {
        const char *c;
        const int8_t *i;
    } code = {
        .c = strstr(texturePath.data, ",") + 1
    };
    unsigned long codeLen = strlen(code.c);

    int padding = 
        (codeLen >= 1 && code.c[codeLen - 1] == '=') +
        (codeLen >= 2 && code.c[codeLen - 2] == '=');

    unsigned long decodedLen = (codeLen / 4) * 3 - padding;

    char decoded[(codeLen * 4) / 4 + 4] = {};
    base64_decode(code.i, codeLen, decoded);

    return loadFromMemoryVanila((struct TextureData) {
        .data = (char *)decoded,
        .qData = decodedLen
    });
}

static struct TextureLoaded loadDefault(struct TextureData) {
    struct TextureLoaded data = {
        .extent.width = 1,
        .extent.height = 1,
        .channels = 1,
        .pixels = malloc(sizeof(char) * 4)
    };

    data.pixels[0] = -1;
    data.pixels[1] = -1;
    data.pixels[2] = -1;
    data.pixels[3] = -1;

    return data;
}

void loadTextureFiles(size_t qTexture, struct TextureData texture[qTexture], struct TextureLoaded loaded[qTexture]) {
    for (size_t i = 0; i < qTexture; i += 1) {
        struct TextureLoaded (*loadTextureInternal)(struct TextureData) =
            texture[i].data == NULL ?                   loadDefault :
            strncmp(texture[i].data, "data:", 5) == 0 ? loadFromMemoryBase64 :
            texture[i].mode == FROM_MEMORY ?            loadFromMemoryVanila :
                                                        loadFromFile;

        loaded[i] = loadTextureInternal(texture[i]);
    }
}

struct BufferObj *loadImagesToBuffer(size_t qTexture, struct TextureLoaded texture[qTexture], struct GraphicsSetup *graphics) {
    VkDeviceSize imageSize = 0; 
    VkDeviceSize prevSize = 0;
    for (size_t i = 0; i < qTexture; i += 1) {
        imageSize += texture[i].extent.width * texture[i].extent.height * 4;
    }

    struct BufferObj *staging = createBufferObj((struct BufferBuilder) {
        .bufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .memoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        .size = imageSize,
        .repetitions = 1
    }, graphics);

    void *dataD;
    vkMapMemory(graphics->device, staging->memory, 0, imageSize, 0, &dataD);

    for (size_t i = 0; i < qTexture; i += 1) {
        imageSize = texture[i].extent.width * texture[i].extent.height * 4;

        memcpy((char *)dataD + prevSize, texture[i].pixels, imageSize);
        stbi_image_free(texture[i].pixels);

        prevSize += imageSize;
    }
    vkUnmapMemory(graphics->device, staging->memory);

    return staging;
}
