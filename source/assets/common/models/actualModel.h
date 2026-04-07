#include <cglm/cglm.h>

#include "buffer.h"

struct GraphicsSetup;

struct MeshInput {
    size_t sizeOfVertex;

    size_t verticesQuantity;
    void *vertices;

    size_t indicesQuantity;
    uint32_t *indices;
};

struct ModelInput {
    struct buffer *buffers;

    size_t qTextures;
    char **inputTextures;

    uint32_t meshQuantity;
    struct MeshInput *mesh;
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

    struct buffer *buffers;

    size_t qTextures;
    char **inputTextures;

    uint32_t meshQuantity;
    struct Mesh *mesh;
};

struct Model *loadModel(const char *filePath, struct GraphicsSetup *graphics);
void destroyActualModel(void *modelPtr);
