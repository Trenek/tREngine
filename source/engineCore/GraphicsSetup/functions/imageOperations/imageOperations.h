VkDeviceMemory createImageMemory(VkDevice device, VkPhysicalDevice physicalDevice, VkImage image, VkMemoryPropertyFlags properties);
void transitionImageLayout(VkImage image, [[maybe_unused]] VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, VkDevice device, VkCommandPool commandPool, VkQueue queue, uint32_t layerCount);
void transitionImageLayout2(VkCommandBuffer commandBuffer, VkImageMemoryBarrier2 *barrier);
void copyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer buffer, VkImage image, VkExtent2D extent, uint32_t layerCount, VkDeviceSize prev);
