#ifndef IMAGE_OBJ_H
#define IMAGE_OBJ_H

#include <vulkan/vulkan_core.h>

struct ImageObj {
    VkDevice device;

    VkImage image;
    VkImageView imageView;
    VkDeviceMemory memory;
    VkSampler sampler;
};

struct ImageBuilder {
    VkExtent2D extent;
    uint32_t mipLevels;
    uint32_t arrayLayers;
    VkFormat format;
    VkImageUsageFlags usage;
    VkFlags flags;
};

struct MipmapsBuilder {
    VkFormat format;
    VkExtent2D extent;
    uint32_t mipLevels;
    VkFilter filter;
};

struct SamplerBuilder {
    VkFilter magFilter;
    uint32_t mipLevels;
    VkSamplerMipmapMode mipMapMode;
    VkBool32 asinotropyEnable;
};

struct GraphicsSetup;
struct ImageObj *createImageObj(struct ImageBuilder builder, struct GraphicsSetup *graphics);
void createImageSampler(struct ImageObj *image, VkDevice device, VkPhysicalDevice physicalDevice, struct SamplerBuilder builder);
void completeImageObj(struct ImageObj *result, struct GraphicsSetup *graphics);
void destroyImageObj(void *bufferPtr);

void generateMipmaps(VkCommandBuffer commandBuffer, VkImage image, struct MipmapsBuilder builder, VkPhysicalDevice physicalDevice);

struct ImageBinder {
    uint32_t qImage;
    struct ImageObj **image;

    VkImageLayout layout;
    VkDescriptorType descriptorType;
};

void bindImagesToDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, struct ImageBinder binder);

#endif
