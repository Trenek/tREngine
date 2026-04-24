#include <cgltf.h>
#include <string.h>

#include "gltf.h"
#include "texture.h"
#include "model.h"

#include "graphicsSetup.h"

#include "bufferOperations.h"

static cgltf_accessor *getAccessor(cgltf_attribute_type type, cgltf_primitive* primitive) {
    cgltf_accessor *result = NULL;

    for (size_t i = 0; result == NULL && i < primitive->attributes_count; i += 1) {
        if (primitive->attributes[i].type == type) {
            result = primitive->attributes[i].data;
        }
    }

    return result;
}

static void readAccessorOr(cgltf_accessor *accessor, int id, float *vec, size_t num, float or) {
    if (accessor) cgltf_accessor_read_float(accessor, id, vec, num);
    else for (size_t i = 0; i < num; i += 1) {
        vec[i] = or;
    }
}

static void loadMorphTargets(struct GltfVertex *vertex, size_t id, cgltf_primitive *primitive) {
    vec3 *morphPos[4] = {
        &vertex[id].morphPos1,
        &vertex[id].morphPos2,
        &vertex[id].morphPos3,
        &vertex[id].morphPos4,
    };

    assert(primitive->targets_count <= 4);

    for (cgltf_size i = 0; i < 4; i += 1) {
        for (size_t j = 0; j < 3; j += 1) {
            (*morphPos[i])[j] = 0;
        }
    }
    for (cgltf_size i = 0; i < primitive->targets_count; i += 1) {
        assert(primitive->targets[i].attributes_count == 1);
        assert(primitive->targets[i].attributes->type == cgltf_attribute_type_position);

        readAccessorOr(primitive->targets[i].attributes->data, id, *morphPos[i], 3, 0);
    }
}

static void loadPrimitive(struct MeshInput *mesh, cgltf_primitive *primitive) {
    cgltf_accessor *indexAccessor = primitive->indices;
    cgltf_accessor *vertexAccessor = getAccessor(cgltf_attribute_type_position, primitive);
    cgltf_accessor *textureAccessor = getAccessor(cgltf_attribute_type_texcoord, primitive);
    cgltf_accessor *colorAccessor = getAccessor(cgltf_attribute_type_color, primitive);
    cgltf_accessor *normalAccessor = getAccessor(cgltf_attribute_type_normal, primitive);
    cgltf_accessor *weightAccessor = getAccessor(cgltf_attribute_type_weights, primitive);
    cgltf_accessor *jointAccessor = getAccessor(cgltf_attribute_type_joints, primitive);

    assert(vertexAccessor != NULL);
    if (NULL != textureAccessor) assert(vertexAccessor->count == textureAccessor->count);
    if (NULL != colorAccessor)   assert(vertexAccessor->count == colorAccessor->count);
    if (NULL != normalAccessor)  assert(vertexAccessor->count == normalAccessor->count);
    if (NULL != weightAccessor)  assert(vertexAccessor->count == weightAccessor->count);
    if (NULL != jointAccessor)   assert(vertexAccessor->count == jointAccessor->count);

    mesh->sizeOfVertex = sizeof(struct GltfVertex);
    mesh->verticesQuantity = vertexAccessor->count;
    mesh->vertices = malloc(sizeof(struct GltfVertex) * mesh->verticesQuantity);
    mesh->indicesQuantity = indexAccessor == NULL ? mesh->verticesQuantity : indexAccessor->count;
    mesh->indices = malloc(sizeof(uint32_t) * mesh->indicesQuantity);

    for (cgltf_size i = 0; i < vertexAccessor->count; i += 1) {
        cgltf_accessor_read_float(vertexAccessor, i, GLTF(mesh->vertices)[i].pos, 3);

        readAccessorOr(textureAccessor, i, GLTF(mesh->vertices)[i].tex, 2, 0);
        readAccessorOr(colorAccessor, i, GLTF(mesh->vertices)[i].color, 3, 1);
        readAccessorOr(normalAccessor, i, GLTF(mesh->vertices)[i].norm, 3, 1);
        readAccessorOr(weightAccessor, i, GLTF(mesh->vertices)[i].weight, 4, 0);
        readAccessorOr(jointAccessor, i, GLTF(mesh->vertices)[i].joint, 4, 0);

        loadMorphTargets(GLTF(mesh->vertices), i, primitive);
    }

    for (cgltf_size i = 0; i < mesh->indicesQuantity; i += 1) {
        mesh->indices[i] = indexAccessor == NULL ? i : cgltf_accessor_read_index(indexAccessor, i);
    }
}

static size_t countMeshContainingNodes(cgltf_data *data) {
    size_t result = 0;

    for (cgltf_size i = 0; i < data->nodes_count; i += 1) {
        result += NULL != data->nodes[i].mesh ? data->nodes[i].mesh->primitives_count : 0;
    }

    return result;
}

static void loadTransformations(mat4 transformations, cgltf_node *node) {
    glm_mat4_identity(transformations);

    if (node->has_matrix) {
        glm_mat4_dup((void *)node->matrix, transformations);
    }

    if (node->has_translation) {
        glm_translate(transformations, node->translation);
    }

    if (node->has_rotation) {
        glm_rotate(transformations, acosf(node->rotation[3]) * 2, node->rotation);
    }

    if (node->has_scale) {
        glm_scale(transformations, node->scale);
    }
}

static struct Frames loadChannel(cgltf_animation_channel *channel) {
    cgltf_animation_sampler *sampler = channel->sampler;

    cgltf_accessor *input = sampler->input;
    cgltf_accessor *output = sampler->output;

    struct Frames result = {
        .qFrames = input->count,
        .qComponents = cgltf_num_components(output->type) * (output->count / input->count),
        .interpolationType = (
            channel->target_path == cgltf_animation_path_type_rotation &&
            sampler->interpolation == cgltf_interpolation_type_linear
        ) ? 3 : sampler->interpolation
    };

    float (*val)[result.qComponents + 1] = (void *)(
        result.values = malloc(result.qFrames * (result.qComponents + 1) * sizeof(float))
    );

    float times[input->count];
    float weights[input->count][result.qComponents];

    cgltf_accessor_unpack_floats(input, times, input->count);
    cgltf_accessor_unpack_floats(output, (void *)weights, input->count * result.qComponents);

    for (cgltf_size i = 0; i < result.qFrames; i += 1) {
        val[i][0] = times[i];
        memcpy(val[i] + 1, weights + i, sizeof(float[result.qComponents]));
    }

    return result;
}

static void *loadAnimations(cgltf_data *data) {
    struct Frames (*frames)[data->nodes_count][cgltf_animation_path_type_max_enum] = calloc(
        data->animations_count * data->nodes_count,
        sizeof(struct Frames[cgltf_animation_path_type_max_enum])
    );

    static_assert(cgltf_animation_path_type_max_enum == ANIM_PATH_TYPE_MAX_ENUM);

    for (cgltf_size i = 0; i < data->animations_count; i += 1) {
        for (cgltf_size j = 0; j < data->animations[i].channels_count; j += 1) {
            size_t nodeID = data->animations[i].channels[j].target_node - data->nodes;
            size_t targetPath = data->animations[i].channels[j].target_path;

            frames[i][nodeID][targetPath] = loadChannel(&data->animations[i].channels[j]);
        }
    }

    return frames;
}

static struct Joint *loadJoints(cgltf_skin *skinData, cgltf_node *node, int *jointID) {
    struct Joint *joint = calloc(skinData->joints_count, sizeof(struct Joint));
    cgltf_accessor *inverseMatrixAccessor = skinData->inverse_bind_matrices;
    mat4 inverseMatrixLoaded[skinData->joints_count];

    cgltf_accessor_unpack_floats(inverseMatrixAccessor, (void *)inverseMatrixLoaded, inverseMatrixAccessor->count * (sizeof(mat4) / sizeof(float)));

    assert(inverseMatrixAccessor->count == skinData->joints_count);
    for (cgltf_size i = 0; i < skinData->joints_count; i += 1) {
        glm_mat4_dup(inverseMatrixLoaded[i], joint[i].inverseMatrix);

        joint[i].nodeID = skinData->joints[i] - node;
        jointID[joint[i].nodeID] = i;
        joint[i].father = -1;
    }

    for (cgltf_size i = 0; i < skinData->joints_count; i += 1) {
        for (cgltf_size j = 0; j < skinData->joints[i]->children_count; j += 1) {
            cgltf_size childNodeID = skinData->joints[i]->children[j] - node;
            
            joint[jointID[childNodeID]].father = i;
        }
    }

    return joint;
}

static struct Skin *loadSkins(cgltf_data *data) {
    struct Skin *skins = calloc(data->skins_count, sizeof(struct Skin));

    for (cgltf_size i = 0; i < data->skins_count; i += 1) {
        skins[i].qNodes = data->nodes_count;
        skins[i].jointID = malloc(sizeof(int) * skins[i].qNodes);
        for (size_t j = 0; j < data->nodes_count; j += 1) {
            skins[i].jointID[j] = -1;
        }

        skins[i].qJoint = data->skins[i].joints_count;
        skins[i].joint = loadJoints(&data->skins[i], data->nodes, skins[i].jointID);
    }

    return skins;
}

static void cleanupGltfModelInfo(void *objInfoPtr) {
    struct GltfModelInfo *objInfo = objInfoPtr;

    if (NULL != objInfo->buffers) {
        for (size_t i = 0; i < objInfo->qBuffer; i += 1) {
            destroyBuffer(objInfo->device, objInfo->buffers[i].buffers, objInfo->buffers[i].buffersMemory);
        }
    }

    for (size_t i = 0; i < objInfo->qAnim * objInfo->qNodes; i += 1) {
        for (size_t j = 0; j < ANIM_PATH_TYPE_MAX_ENUM; j += 1) {
            free(objInfo->frames[i][j].values);
        }
    }

    for (size_t i = 0; i < objInfo->qSkin; i += 1) {
        free(objInfo->skin[i].jointID);
        free(objInfo->skin[i].joint);
    }
    free(objInfo->skin);
    free(objInfo->frames);
    free(objInfo->nodes);

    free(objInfo->buffers);
    free(objInfo->pushConstants);

    free(objInfo);
} 

#define GET_OR_DEF(X, ...) \
    data->nodes[i].has_##X ? \
    data->nodes[i].X : (vec4) { __VA_ARGS__ }

static struct NodeData *loadNodes(cgltf_data *data, struct GltfModelInfo *info, struct ModelInput *model) {
    struct NodeData *nodes = malloc(data->nodes_count * sizeof(struct NodeData));

    size_t loadedMesh = 0;

    for (cgltf_size i = 0; i < data->nodes_count; i += 1) {
        memcpy(nodes[i].translation, GET_OR_DEF(translation, 0, 0, 0), sizeof(float[3]));
        memcpy(nodes[i].scale, GET_OR_DEF(scale, 1, 1, 1), sizeof(float[3]));
        memcpy(nodes[i].rotation, GET_OR_DEF(rotation, 0, 0, 0, 1), sizeof(float[4]));

        if (NULL != data->nodes[i].mesh) {
            for (cgltf_size j = 0; j < data->nodes[i].mesh->primitives_count; j += 1) {
                loadPrimitive(&model->mesh[loadedMesh], &data->nodes[i].mesh->primitives[j]);

                GLTF_PC(info->pushConstants)[loadedMesh].nodeID = i;
                GLTF_PC(info->pushConstants)[loadedMesh].materialID = data->nodes[i].mesh->primitives[j].material == NULL ? 0 : cgltf_material_index(
                    data,
                    data->nodes[i].mesh->primitives[j].material
                );

                loadedMesh += 1;
            }
        }

        for (size_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j += 1) {
            loadTransformations(((mat4**)info->buffers[0].buffersMapped)[j][i], &data->nodes[i]);
            glm_mat4_identity(((struct AnimationData**)info->buffers[1].buffersMapped)[j][i].animation);
        }
    }

    return nodes;
}

#undef GET_OR_DEF

struct Materials {
	cgltf_float base_color_factor[4];
	cgltf_float metallic_factor;
	cgltf_float roughness_factor;
    cgltf_int baseColorID;
    cgltf_float pad1;
};

static void loadMaterials(cgltf_data *data, struct Materials *materials[MAX_FRAMES_IN_FLIGHT]) {
    vec4 id = { 1.0f, 1.0f, 1.0f, 1.0f };

    if (0 == data->materials_count) {
        for (uint32_t k = 0; k < MAX_FRAMES_IN_FLIGHT; k += 1) {
            memcpy(materials[k]->base_color_factor, id, sizeof(float[4]));
            
            materials[k]->metallic_factor = 0;
            materials[k]->roughness_factor = 0;
        }
    }
    else for (size_t i = 0; i < data->materials_count; i += 1) {
        for (uint32_t k = 0; k < MAX_FRAMES_IN_FLIGHT; k += 1) {
            memcpy(materials[k][i].base_color_factor, data->materials[i].pbr_metallic_roughness.base_color_factor, sizeof(float[4]));
            
            materials[k][i].baseColorID = data->materials[i].pbr_metallic_roughness.base_color_texture.texture == NULL ? 0 : cgltf_texture_index(
                data, 
                data->materials[i].pbr_metallic_roughness.base_color_texture.texture
            );
            materials[k][i].metallic_factor = data->materials[i].pbr_metallic_roughness.metallic_factor;
            materials[k][i].roughness_factor = data->materials[i].pbr_metallic_roughness.roughness_factor;
        }
    }
}

static void loadGltfTextures(struct ModelInput *model, cgltf_data *obj) {
    for (size_t i = 0; i < obj->textures_count; i += 1) {
        model->texture[i].mode = FROM_OTHER;

        if (obj->textures[i].image->buffer_view) {
            model->texture[i].qData = obj->textures[i].image->buffer_view->size;
            model->texture[i].data = malloc(model->texture[i].qData);
            memcpy(
                model->texture[i].data,
                (char *)obj->textures[i].image->buffer_view->buffer->data +
                obj->textures[i].image->buffer_view->offset,
                model->texture[i].qData
            );
            model->texture[i].mode = FROM_MEMORY;
        }
        else if (obj->textures[i].image->uri == NULL) {
            model->texture[i].data = NULL;
        }
        else {
            model->texture[i].data = strdup(obj->textures[i].image->uri);
        }
    }
}

static size_t max(size_t a, size_t b) {
    return a > b ? a : b;
}

void gltfLoadModel(const char *modelPath, struct ModelInput *model, struct GraphicsSetup *graphics) {
    cgltf_options options = {};
    cgltf_data *data = NULL;

    if (cgltf_result_success == cgltf_parse_file(&options, modelPath, &data))
    if (cgltf_result_success == cgltf_load_buffers(&options, data, modelPath)) {
        struct GltfModelInfo *info = model->info = malloc(sizeof(struct GltfModelInfo));

        model->cleanup = cleanupGltfModelInfo,
        model->meshQuantity = countMeshContainingNodes(data);
        model->mesh = malloc(sizeof(struct Mesh) * model->meshQuantity);

        info->device = graphics->device;
        info->pushConstants = malloc(sizeof(struct GltfPushConstants) * data->nodes_count);
        info->qBuffer = 3;
        info->buffers = malloc(info->qBuffer * sizeof(struct buffer));
        info->buffers[0].range = data->nodes_count * sizeof(mat4);
        info->buffers[1].range = data->nodes_count * sizeof(struct AnimationData);
        info->buffers[2].range = max(data->materials_count, 1) * sizeof(struct Materials);
        model->qTexture = max(data->textures_count, 1);
        model->texture = calloc(model->qTexture, sizeof(struct TextureData));

        createBuffers(
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            info->buffers[0].range,
            &info->buffers[0].buffers, 
            &info->buffers[0].buffersMemory, 
            info->buffers[0].buffersMapped, 
            graphics->device, 
            graphics->physicalDevice, 
            graphics->surface
        );

        createBuffers(
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            info->buffers[1].range,
            &info->buffers[1].buffers, 
            &info->buffers[1].buffersMemory, 
            info->buffers[1].buffersMapped, 
            graphics->device, 
            graphics->physicalDevice, 
            graphics->surface
        );

        createBuffers(
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            info->buffers[2].range,
            &info->buffers[2].buffers, 
            &info->buffers[2].buffersMemory, 
            info->buffers[2].buffersMapped, 
            graphics->device, 
            graphics->physicalDevice, 
            graphics->surface
        );

        info->nodes = loadNodes(data, info, model);

        loadMaterials(data, (void *)info->buffers[2].buffersMapped);
        loadGltfTextures(model, data);

        info->qAnim = data->animations_count;
        info->qNodes = data->nodes_count;
        info->frames = loadAnimations(data);

        info->qSkin = data->skins_count;
        info->skin = loadSkins(data);
    }

    cgltf_free(data);
}
