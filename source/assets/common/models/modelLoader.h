struct GraphicsSetup;
struct ModelInput;

void ttfLoadModel(const char *objectPath, struct ModelInput *model, struct GraphicsSetup *);
void objLoadModel(const char *objectPath, struct ModelInput *model, struct GraphicsSetup *);
void gltfLoadModel(const char *objectPath, struct ModelInput *model, struct GraphicsSetup *);
void recLoadModel(const char *, struct ModelInput *model, struct GraphicsSetup *);
