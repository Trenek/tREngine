VkDeviceMemory createImageMemory(VkDevice device, VkPhysicalDevice physicalDevice, VkImage image, VkMemoryPropertyFlags properties);
void transitionImageLayout(VkImage image, [[maybe_unused]] VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, VkDevice device, VkCommandPool commandPool, VkQueue queue, uint32_t layerCount);
void transitionImageLayout2(VkImageMemoryBarrier2 *barrier, VkDevice device, VkCommandPool commandPool, VkQueue queue);
void copyBufferToImage(VkBuffer buffer, VkImage image, VkExtent2D extent, VkDevice device, VkCommandPool commandPool, VkQueue queue, uint32_t layerCount);
void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t width, int32_t height, uint32_t mipLevels, VkFilter imageFilter, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue queue);
VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, VkImageViewType viewType, uint32_t layerCount);
