#include "descriptorSetLayoutObj.h"

struct GraphicsSetup;

struct TextureData {
    size_t qData;
    char *data;

    enum TextureMode {
        FROM_OTHER,
        FROM_MEMORY,
    } mode;
};

struct Textures {
    VkDevice device;
    
    struct DescriptorSetLayout *descriptorLayout;
    struct DescriptorObj *descriptor;

    size_t quantity;
    struct Data {
        VkImage image;
        VkDeviceMemory imageMemory;
        VkImageView imageView;
        VkSampler sampler;
    } *data;
};

struct Textures *loadCubeMaps(struct GraphicsSetup *graphics, const char *texturePath[6]);
struct Textures *loadTextures(struct GraphicsSetup *graphics, uint32_t texturesQuantity, struct TextureData texturePath[static texturesQuantity]);
struct Textures *loadUintTextures(struct GraphicsSetup *graphics, uint32_t texturesQuantity, struct TextureData texturePath[static texturesQuantity]);
void unloadTextures(void *texturePtr);
