#include <cglm.h>

#include "buffer.h"

struct GraphicsSetup;

struct NodeTransform {
    mat4 matrix;
    vec4 weight;
};

struct Mesh {
    void *vertices;
    size_t sizeOfVertex;
    uint16_t *indices;
    size_t verticesQuantity;
    size_t indicesQuantity;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
};

struct timeFrame {
    size_t qData;
    size_t qValues;
    int interpolationType;

    struct timePoint {
        float time;
        float *values;
    } *data;
};

struct skinData {
    struct jointData2 {
        mat4 inverseMatrix;

        int nodeID;
        int father;
    } *jointData;

    int *jointID;
};

struct colisionBox {
    char *name;

    size_t qVertex;
    void **vertex;
};

struct actualModel {
    VkDevice device;

    struct buffer localMesh;

    size_t qNode;
    size_t qAnim;
    size_t qSkin;
    struct timeFrame (*anim)[4];
    struct skinData *skin;

    uint32_t meshQuantity;
    struct Mesh *mesh;

    size_t qHitbox;
    size_t qHurtBox;

    struct colisionBox *hitBox;
    struct colisionBox *hurtBox;

    char *textureData;
};

struct actualModel *loadModel(const char *filePath, struct GraphicsSetup *graphics);
void destroyActualModel(void *modelPtr);

struct Entity;
void animate(struct Entity *model, struct actualModel *actualModel, size_t animID, float deltaTime);
