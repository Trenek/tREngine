#include <vulkan/vulkan_core.h>

struct GraphicsSetup;

struct MeshInput {
    size_t sizeOfVertex;

    size_t verticesQuantity;
    void *vertices;

    size_t indicesQuantity;
    uint32_t *indices;
};

struct ModelInput {
    uint32_t meshQuantity;
    struct MeshInput *mesh;

    size_t qTextures;
    char **inputTextures;

    void *info;
    void (*cleanup)(void *);
};

struct Mesh {
    size_t indicesQuantity;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
};

struct Model {
    VkDevice device;

    uint32_t meshQuantity;
    struct Mesh *mesh;

    size_t qTextures;
    char **inputTextures;

    void *info;
    void (*cleanup)(void *);
};

struct Model *loadModel(const char *filePath, struct GraphicsSetup *graphics);
void destroyActualModel(void *modelPtr);
