struct GraphicsSetup;
struct ModelInput;

void ttfLoadModel(const char *objectPath, struct ModelInput *model, struct GraphicsSetup *);
void objLoadModel(const char *objectPath, struct ModelInput *model, struct GraphicsSetup *);
void gltfLoadModel(const char *objectPath, struct ModelInput *model, struct GraphicsSetup *);
void recLoadModel(const char *, struct ModelInput *model, struct GraphicsSetup *);
void rec2LoadModel(const char *, struct ModelInput *model, struct GraphicsSetup *);
void screenLoadModel(const char *, struct ModelInput *model, struct GraphicsSetup *);

struct BufferObj *createVertexBuffer(struct GraphicsSetup *graphics, uint32_t vertexQuantity, void *vertices, size_t sizeOfBuffer);
struct BufferObj *createIndexBuffer(struct GraphicsSetup *graphics, uint32_t indicesQuantity, uint32_t indices[static indicesQuantity]);

