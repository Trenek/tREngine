#include <stb_image.h>

#include "texture.h"

struct TextureLoaded {
    stbi_uc *pixels;

    VkExtent3D extent;
    int channels;
};

void loadTextureFiles(size_t qTexture, struct TextureData texture[qTexture], struct TextureLoaded loaded[qTexture]);
struct BufferObj *loadImagesToBuffer(size_t qTexture, struct TextureLoaded texture[qTexture], struct GraphicsSetup *graphics);

void copyTextureBufferPixels(VkCommandBuffer commandBuffer, VkImage textureImage, uint32_t mipLevels, struct BufferObj *staging, VkExtent3D extent, VkFilter textureFilter, struct GraphicsSetup *graphics, VkDeviceSize prev);
void copyCubeMapTexture(VkCommandBuffer commandBuffer, VkImage textureImage, uint32_t mipLevels, struct BufferObj *staging, VkExtent2D extent);
