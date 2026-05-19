#include "texture.h"

VkDescriptorSetLayout createTextureDescriptorSetLayout(VkDevice device, uint32_t textureQuantity);
VkDescriptorPool createTextureDescriptorPool(VkDevice device, uint32_t texturesCount);
void bindTextureBuffersToDescriptorSets(VkDescriptorSet descriptorSets[], VkDevice device, uint32_t texturesQuantity, struct Textures *texture);

VkImage createTextureBuffer(VkDeviceMemory *textureImageMemory, uint32_t *mipLevels, struct TextureData texturePath, VkFormat textureFormat, VkFilter textureFilter, struct GraphicsSetup *graphics);
VkSampler createTextureSampler(VkDevice device, VkPhysicalDevice physicalDevice, VkFilter magFilter, uint32_t mipLevels, VkSamplerMipmapMode mipMapMode, VkBool32 asinotropyEnable);
