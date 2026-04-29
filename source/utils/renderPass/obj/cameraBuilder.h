#include <stddef.h>

typedef struct VkExtent2D VkExtent2D;

struct cameraBuilder {
    size_t size;
    size_t bufferSize;
    const void *mapped;
    void (*updateBuffer)(void *buffersMapped, VkExtent2D swapChainExtent, void *);
};
