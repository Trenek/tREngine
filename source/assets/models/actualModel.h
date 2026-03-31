#include <cglm.h>

#include "buffer.h"

struct GraphicsSetup;

struct MeshInput {
    void *vertices;
    size_t sizeOfVertex;
    uint16_t *indices;
    size_t verticesQuantity;

    size_t indicesQuantity;
};

struct ModelInput {
    struct buffer localMesh;

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

    struct buffer localMesh;

    uint32_t meshQuantity;
    struct Mesh *mesh;
};

struct Model *loadModel(const char *filePath, struct GraphicsSetup *graphics);
void destroyActualModel(void *modelPtr);
