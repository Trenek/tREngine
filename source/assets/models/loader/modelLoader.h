#include "actualModel.h"

struct GraphicsSetup;
struct Model;

void ttfLoadModel(const char *objectPath, struct ModelInput *model, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

struct Model *loadModel(const char *filePath, struct GraphicsSetup *vulkan);

void destroyActualModel(void *modelPtr);
