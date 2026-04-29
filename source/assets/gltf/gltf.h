#include "Vertex.h"
#include "buffer.h"

#define GLTF(X) ((struct GltfVertex *)X)
#define GLTF_PC(X) ((struct GltfPushConstants *)X)

#define ANIM_PATH_TYPE_MAX_ENUM 5

struct Frames {
    size_t qFrames;
    size_t qComponents;

    size_t interpolationType;

    float *values;
};

struct NodeData {
    mat4 mat;

    vec3 translation;
    vec3 scale;
    vec4 rotation;
};

struct Joint {
    mat4 inverseMatrix;

    int nodeID;
    int father;
};

struct Skin {
    size_t qNodes;
    int *jointID;

    size_t qJoint;
    struct Joint *joint;
};

struct AnimationData {
    mat4 animation;
    float weight1;
    float weight2;
    float weight3;
    float weight4;

    int jointToNodeID;
    int pad0;
    int pad1;
    int pad2;
};

struct ColisionBox {
    char *name;

    size_t qVertex;
    struct GltfVertex *vertex;
};

struct Entity;
struct Model;
void animate(struct Entity *, struct Model *, size_t, float);
struct descriptorSetLayout *defaultGltfDescriptorSetLayout(VkDevice device);

struct GltfModelInfo {
    VkDevice device;

    size_t qBuffer;
    struct buffer *buffers;

    size_t qAnim;
    size_t qNodes;
    struct Frames (*frames)[ANIM_PATH_TYPE_MAX_ENUM];
    struct NodeData *nodes;

    size_t qSkin;
    struct Skin *skin;

    void *pushConstants;

    size_t qHitbox;
    size_t qHurtBox;

    struct ColisionBox *hitBox;
    struct ColisionBox *hurtBox;
};

struct GltfPushConstants {
    uint32_t nodeID;
    uint32_t materialID;
};

struct GltfVertex {
    vec3 pos;
    vec3 norm;
    vec2 tex;
    vec3 color;
    vec4 weight;
    vec4 joint;

    vec3 morphPos1;
    vec3 morphPos2;
    vec3 morphPos3;
    vec3 morphPos4;
};

static inline struct Vert defaultGltfVert() {
    static const VkVertexInputAttributeDescription vertexAttributeDescriptions[] = {
        [0] = {
            .binding = 0,
            .location = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(struct GltfVertex, pos)
        },
        [1] = {
            .binding = 0,
            .location = 1,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(struct GltfVertex, norm)
        },
        [2] = {
            .binding = 0,
            .location = 2,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(struct GltfVertex, tex)
        },
        [3] = {
            .binding = 0,
            .location = 3,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(struct GltfVertex, color)
        },
        [4] = {
            .binding = 0,
            .location = 4,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(struct GltfVertex, weight)
        },
        [5] = {
            .binding = 0,
            .location = 5,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(struct GltfVertex, joint)
        },
        [6] = {
            .binding = 0,
            .location = 6,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(struct GltfVertex, morphPos1)
        },
        [7] = {
            .binding = 0,
            .location = 7,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(struct GltfVertex, morphPos2)
        },
        [8] = {
            .binding = 0,
            .location = 8,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(struct GltfVertex, morphPos3)
        },
        [9] = {
            .binding = 0,
            .location = 9,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(struct GltfVertex, morphPos4)
        },
    };

    return (struct Vert) {
        .sizeOfVertex = sizeof(struct GltfVertex),
        .numOfAttributes = sizeof(vertexAttributeDescriptions) / sizeof(VkVertexInputAttributeDescription),
        .attributeDescription = vertexAttributeDescriptions
    };
}
