#define CGLTF_IMPLEMENTATION

#include <vulkan/vulkan.h>
#include <cglm.h>
#include <cgltf.h>
#include <string.h>

#include "MY_ASSERT.h"
#include "actualModel.h"

#include "Vertex.h"

#define BFR(x) ((struct GLTFVertex *)(x))

#define GLTF_X 2
#define GLTF_Y 0
#define GLTF_Z 1

mat4 changeMat4 = {
    { 0, 0, 1, 0 },
    { 1, 0, 0, 0 },
    { 0, 1, 0, 0 },
    { 0, 0, 0, 1 },
};

const mat3 changeMat3 = {
    { 0, 0, 1 },
    { 1, 0, 0 },
    { 0, 1, 0 },
};

static void *getBufferOffset(cgltf_buffer_view *bufferView) {
    return (uint8_t *)bufferView->buffer->data + bufferView->offset;
}

static void *getAccessorOffset(cgltf_accessor *accessor) {
    return (uint8_t *)getBufferOffset(accessor->buffer_view) + accessor->offset;
}

static void applySparce(cgltf_accessor *accessor, void *dest, size_t size) {
    uint16_t *indices = NULL;
    uint8_t *newVal = NULL;

    if (accessor->is_sparse) {
        indices = getBufferOffset(accessor->sparse.indices_buffer_view);
        newVal = getBufferOffset(accessor->sparse.values_buffer_view);

        for (uint16_t i = 0; i < accessor->sparse.count; i += 1) {
            memcpy((uint8_t *)dest + size * indices[i], newVal + size * i, size);
        }
    }
}

static int countMeshes(uint16_t n, cgltf_node x[n]) {
    int quantity = 0;

    for (uint16_t i = 0; i < n; i += 1) {
        quantity += x[i].mesh != NULL;
    }

    return quantity;
}

static cgltf_accessor *getAccessor(cgltf_attribute_type type, cgltf_primitive* primitive) {
    cgltf_accessor *result = NULL;

    for (size_t i = 0; result == NULL && i < primitive->attributes_count; i += 1) {
        if (primitive->attributes[i].type == type) {
            result = primitive->attributes[i].data;
        }
    }

    return result;
}

static void loadFromAccessor(cgltf_accessor *accessor, void *local, size_t size, uint16_t quantity) {
    if (accessor != NULL) {
        if (accessor->stride == 0 || accessor->stride == size) {
            memcpy(local, getAccessorOffset(accessor), size * quantity);
        }
        else for (cgltf_size i = 0; i < quantity; i += 1) {
            memcpy((char *)local + i * size, (char *)getAccessorOffset(accessor) + i * accessor->stride, size);
        }
        applySparce(accessor, local, size);
    }
}

static void loadIndicesFromAccessor(cgltf_accessor *accessor, uint16_t *local, uint16_t quantity) {
    if (accessor != NULL) {
        loadFromAccessor(accessor, local, sizeof(uint16_t), quantity);
    }
    else for (uint16_t i = 0; i < quantity; i += 1) {
        local[i] = i;
    }
}

static struct Mesh loadMesh(cgltf_mesh *mesh) {
    cgltf_primitive *primitive = mesh->primitives;
    
    cgltf_accessor *index_accessor = primitive->indices;
    cgltf_accessor *vertex_accessor = getAccessor(cgltf_attribute_type_position, primitive);
    cgltf_accessor *texture_accessor = getAccessor(cgltf_attribute_type_texcoord, primitive);
    cgltf_accessor *color_accessor = getAccessor(cgltf_attribute_type_color, primitive);
    cgltf_accessor *normal_accessor = getAccessor(cgltf_attribute_type_normal, primitive);
    cgltf_accessor *joint_accessor = getAccessor(cgltf_attribute_type_joints, primitive);
    cgltf_accessor *weight_accessor = getAccessor(cgltf_attribute_type_weights, primitive);

    struct Mesh result = {
        .indicesQuantity = (index_accessor == NULL ? vertex_accessor : index_accessor)->count,
        .verticesQuantity = vertex_accessor->count,
    };

    result.vertices = calloc(result.verticesQuantity, sizeof(struct GLTFVertex));
    result.indices = calloc(result.indicesQuantity, sizeof(uint16_t));

    float localPosition[result.verticesQuantity][3];
    float localTexture[result.verticesQuantity][2];
    float localColor[result.verticesQuantity][3];
    float localNormal[result.verticesQuantity][3];

    loadIndicesFromAccessor(index_accessor, result.indices, result.indicesQuantity);
    loadFromAccessor(vertex_accessor, localPosition, sizeof(float[3]), result.verticesQuantity);
    loadFromAccessor(texture_accessor, localTexture, sizeof(float[2]), result.verticesQuantity);
    loadFromAccessor(color_accessor, localColor, sizeof(float[3]), result.verticesQuantity);
    loadFromAccessor(normal_accessor, localNormal, sizeof(float[3]), result.verticesQuantity);

    for (size_t i = 0; i < result.verticesQuantity; i += 1) {
        cgltf_uint joints[4] = {};
        cgltf_float weights[4] = { 1 };
        if (joint_accessor && weight_accessor) {
            cgltf_accessor_read_uint(joint_accessor, i, joints, 4);
            cgltf_accessor_read_float(weight_accessor, i, weights, 4);
        }

        BFR(result.vertices)[i] = (struct GLTFVertex) {
            .pos = {
                [0] = vertex_accessor == NULL ? 0.0f : localPosition[i][GLTF_X],
                [1] = vertex_accessor == NULL ? 0.0f : localPosition[i][GLTF_Y],
                [2] = vertex_accessor == NULL ? 0.0f : localPosition[i][GLTF_Z]
            },
            .norm = {
                [0] = normal_accessor == NULL ? 1.0f : localNormal[i][GLTF_X],
                [1] = normal_accessor == NULL ? 1.0f : localNormal[i][GLTF_Y],
                [2] = normal_accessor == NULL ? 1.0f : localNormal[i][GLTF_Z],
            },
            .texCoord = {
                [0] = texture_accessor == NULL ? 0.0f : localTexture[i][0],
                [1] = texture_accessor == NULL ? 0.0f : localTexture[i][1]
            },
            .color = {
                [0] = color_accessor == NULL ? 1.0f : localColor[i][0],
                [1] = color_accessor == NULL ? 1.0f : localColor[i][1],
                [2] = color_accessor == NULL ? 1.0f : localColor[i][2]
            },
            .bone = { joints[0], joints[1], joints[2], joints[3] },
            .weights = { weights[0], weights[1], weights[2], weights[3] }
        };
    }

    return result;
}

static void loadTransformations(mat4 transformations, cgltf_node *node) {
    glm_mat4_identity(transformations);

    if (node->has_matrix) {
        memcpy(transformations, node->matrix, sizeof(mat4));
    }

    if (node->has_translation) {
        float a[3] = {
            node->translation[GLTF_X],
            node->translation[GLTF_Y],
            node->translation[GLTF_Z]
        };

        glm_translate(transformations, a);
    }

    if (node->has_rotation) {
        float a[3] = {
            node->rotation[GLTF_X],
            node->rotation[GLTF_Y],
            node->rotation[GLTF_Z]
        };

        glm_rotate(transformations, acosf(node->rotation[3]) * 2, a);
    }

    if (node->has_scale) {
        float a[3] = {
            node->scale[GLTF_X],
            node->scale[GLTF_Y],
            node->scale[GLTF_Z]
        };

        glm_scale(transformations, a);
    }
}

struct timeFrame loadSamplerInfo(cgltf_animation_sampler *sampler) {
    cgltf_accessor *input = sampler->input;
    cgltf_accessor *output = sampler->output;

    cgltf_size num_components = cgltf_num_components(output->type);
    float temp[num_components];

    struct timeFrame result = {
        .qData = input->count,
        .qValues = num_components,
        .interpolationType = sampler->interpolation,

        .data = calloc(input->count, sizeof(struct timePoint))
    };

    result.data->values = calloc(result.qData * result.qValues, sizeof(float));
    for (cgltf_size k = 0; k < input->count; k++) {
        result.data[k].values = result.data->values + k * num_components;

        MY_ASSERT(cgltf_accessor_read_float(input, k, &result.data[k].time, 1));
        MY_ASSERT(cgltf_accessor_read_float(output, k, temp, num_components));

        memcpy(result.data[k].values, temp, sizeof(float[num_components]));

        printf("%zu, %zu\n", result.qValues, result.qData);
        // is not weight
        if (result.qValues >= 3) {
            result.data[k].values[0] = temp[GLTF_X];
            result.data[k].values[1] = temp[GLTF_Y];
            result.data[k].values[2] = temp[GLTF_Z];
        }
        else if (result.qValues == 2) {
            result.data[k].values[0] = temp[0];
            result.data[k].values[1] = temp[1];

            printf("\t(%f, %f)\n", temp[0], temp[1]);
        }
        else if (result.qValues == 1) {
            result.data[k].values[0] = temp[0];

            printf("\t(%f)\n", temp[0]);
        }
    }

    return result;
}

[[maybe_unused]]
static void loadSkinData(cgltf_data *data, struct skinData skinData[data->skins_count]) {
    for (size_t i = 0; i < data->skins_count; i += 1) {
        cgltf_accessor *inverse_matrix_accessor = data->skins[i].inverse_bind_matrices;
        mat4 inverseMatrix[data->skins[i].joints_count];

        loadFromAccessor(inverse_matrix_accessor, inverseMatrix, sizeof(mat4), data->skins[i].joints_count);

        skinData[i].jointData = malloc(sizeof(struct jointData2) * data->skins[i].joints_count);  
        skinData[i].jointID = malloc(sizeof(int) * data->nodes_count);
            
        for (cgltf_size j = 0; j < data->skins[i].joints_count; j += 1) {
            memcpy(skinData[i].jointData[j].inverseMatrix, inverseMatrix[j], sizeof(mat4));

            skinData[i].jointData[j].nodeID = data->skins[i].joints[j] - data->nodes;
            skinData[i].jointData[j].father = -1;
        }

        for (int j = 0; j < (int)data->nodes_count; j += 1) {
            cgltf_size k = 0;

            while (k < data->skins[i].joints_count && skinData->jointData[k].nodeID != j) k += 1;

            skinData->jointID[j] = (k == data->skins[i].joints_count) ? -1 : (int)k;
        }

        for (cgltf_size j = 0; j < data->skins[i].joints_count; j += 1) {
            for (cgltf_size k = 0; k < data->skins[i].joints[j]->children_count; k += 1) {
                cgltf_size childNodeID = data->skins[i].joints[j]->children[k] - data->nodes;
                cgltf_size childJointID = skinData->jointID[childNodeID];

                skinData->jointData[childJointID].father = j;
            }
        }
    }
}

static void loadNodeData(cgltf_animation_channel *channel, cgltf_node *first, struct timeFrame (*foo2)[4]) {
    cgltf_size i = channel->target_node - first;
    int j = channel->target_path - 1;

    foo2[i][j] = loadSamplerInfo(channel->sampler);

    if (channel->target_path == cgltf_animation_path_type_rotation &&
        foo2[i][j].interpolationType == cgltf_interpolation_type_linear) {
        foo2[i][j].interpolationType = 3;
    }
}

[[maybe_unused]]
static void loadAnimation(cgltf_data *data, struct timeFrame anim[data->animations_count][data->nodes_count][4]) {
    for (cgltf_size i = 0; i < data->animations_count; i += 1) {
        for (cgltf_size j = 0; j < data->animations[i].channels_count; j += 1) {
            loadNodeData(&data->animations[i].channels[j], data->nodes, anim[i]);
        }
    }
}

static size_t countNames(cgltf_size qNode, cgltf_node node[qNode], const char *buffer) {
    size_t result = 0;

    for (cgltf_size i = 0; i < qNode; i += 1) {
        result += (node[i].name != NULL) && (0 == strncmp(node[i].name, buffer, strlen(buffer)));
    }

    return result;
}

static size_t countVertex(size_t qVert, struct GLTFVertex vert[qVert]) {
    size_t i = 0;
    for (size_t k = 0; k < qVert; k += 1) {
        i += (k == 0 ||
            vert[k - 1].pos[0] != vert[k].pos[0] ||
            vert[k - 1].pos[1] != vert[k].pos[1] ||
            vert[k - 1].pos[2] != vert[k].pos[2]
        );
    }

    return i;
}

static void loadPlanes(void **planes, size_t qVert, struct GLTFVertex vert[qVert]) {
    size_t n = 0;

    for (size_t i = 0; i < qVert; i += 1) {
        if (i == 0 ||
            vert[i - 1].pos[0] != vert[i].pos[0] ||
            vert[i - 1].pos[1] != vert[i].pos[1] ||
            vert[i - 1].pos[2] != vert[i].pos[2]
        ) {
            planes[n] = &vert[i];
            for (size_t j = 0; j < n; j += 1) {
                if (BFR(planes[n])->pos[1] > BFR(planes[j])->pos[1]) {
                    void *temp = planes[n];
                    planes[n] = planes[j];
                    planes[j] = temp;
                }
            }

            n += 1;
        }
    }
}

static size_t addColisionBox(struct colisionBox *box, cgltf_node *node, struct Mesh *mesh, const char *name) {
    size_t result = 0;

    if (0 == strncmp(node->name, name, strlen(name))) {
        box->qVertex = countVertex(mesh->verticesQuantity, mesh->vertices);
        box->vertex = calloc(box->qVertex, sizeof(void *));
        loadPlanes(box->vertex, mesh->verticesQuantity, mesh->vertices);

        box->name = calloc(strlen(node->name) + 1, sizeof(char));
        strcpy(box->name, node->name);

        result = 1;
    }
    
    return result;
}

void gltfLoadModel(const char *filePath, struct actualModel *model, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    cgltf_options options = { 0 };
    cgltf_data *data = NULL;

    if (cgltf_result_success == cgltf_parse_file(&options, filePath, &data))
    if (cgltf_result_success == cgltf_load_buffers(&options, data, filePath)) {
        model->meshQuantity = countMeshes(data->nodes_count, data->nodes);
        model->mesh = calloc(model->meshQuantity, sizeof(struct Mesh));

        if (data->images_count > 0) {
            model->textureData = calloc(strlen(data->images[0].uri) + 1, sizeof(char));
            memcpy(model->textureData, data->images[0].uri, sizeof(char) * strlen(data->images[0].uri));
        }
        createBuffers(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, model->meshQuantity * sizeof(mat4), model->localMesh.buffers, model->localMesh.buffersMemory, model->localMesh.buffersMapped, device, physicalDevice, surface);

        int i = 0;
        int z[2] = {};

        model->qHitbox = countNames(data->nodes_count, data->nodes, "Hit");
        model->qHurtBox = countNames(data->nodes_count, data->nodes, "Hurt");

        model->hitBox = calloc(model->qHitbox, sizeof(struct colisionBox));
        model->hurtBox = calloc(model->qHurtBox, sizeof(struct colisionBox));
        for (uint32_t j = 0; j < data->nodes_count; j += 1) if (data->nodes[j].mesh != NULL) {
            model->mesh[i] = loadMesh(data->nodes[j].mesh);
            model->mesh[i].sizeOfVertex = sizeof(struct GLTFVertex);

            if (data->nodes[j].name) {
                z[0] += addColisionBox(&model->hitBox[z[0]], &data->nodes[j], &model->mesh[i], "Hit");
                z[1] += addColisionBox(&model->hurtBox[z[1]], &data->nodes[j], &model->mesh[i], "Hurt");
            }

            for (uint32_t k = 0; k < MAX_FRAMES_IN_FLIGHT; k += 1) {
                loadTransformations(((mat4 **)model->localMesh.buffersMapped)[k][i], &data->nodes[j]);
            }

            i += 1;
        }

        model->qAnim = data->animations_count;
        model->qSkin = data->skins_count;
        model->qNode = data->nodes_count;
        model->anim = NULL;
        model->skin = NULL;

        if (model->qAnim && model->qNode) {
            model->anim = calloc(model->qAnim * model->qNode, sizeof(struct timeFrame[4]));
            loadAnimation(data, (void *)model->anim);
        }
        if (model->qSkin) {
            model->skin = calloc(model->qSkin, sizeof(struct skinData));
            loadSkinData(data, model->skin);
        }

        cgltf_free(data);
    }
}
