#include "actualModel.h"

struct GraphicsSetup;
struct Model;

void ttfLoadModel(const char *objectPath, struct ModelInput *model, struct GraphicsSetup *);
void objLoadModel(const char *objectPath, struct ModelInput *model, struct GraphicsSetup *);
void gltfLoadModel(const char *objectPath, struct ModelInput *model, struct GraphicsSetup *);
void recLoadModel(const char *, struct ModelInput *model, struct GraphicsSetup *);

struct Model *loadModel(const char *filePath, struct GraphicsSetup *vulkan);

void destroyActualModel(void *modelPtr);
