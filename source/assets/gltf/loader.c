#include <cgltf.h>
#include <string.h>

#include "gltf.h"

#include "graphicsSetup.h"
#include "modelLoader.h"

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

static void loadPrimitive(struct MeshInput *mesh, cgltf_primitive *primitive) {
    mesh->sizeOfVertex = sizeof(struct GltfVertex);

    cgltf_accessor *indexAccessor = primitive->indices;
    cgltf_accessor *vertexAccessor = getAccessor(cgltf_attribute_type_position, primitive);
    cgltf_accessor *textureAccessor = getAccessor(cgltf_attribute_type_texcoord, primitive);
    cgltf_accessor *colorAccessor = getAccessor(cgltf_attribute_type_color, primitive);
    cgltf_accessor *normalAccessor = getAccessor(cgltf_attribute_type_normal, primitive);

    if (NULL != textureAccessor) assert(vertexAccessor->count == textureAccessor->count);
    if (NULL != colorAccessor)   assert(vertexAccessor->count == colorAccessor->count);
    if (NULL != normalAccessor)  assert(vertexAccessor->count == normalAccessor->count);

    mesh->verticesQuantity = vertexAccessor->count;
    mesh->vertices = malloc(sizeof(struct GltfVertex) * mesh->verticesQuantity);

    for (cgltf_size i = 0; i < vertexAccessor->count; i += 1) {
        cgltf_accessor_read_float(vertexAccessor, i, GLTF(mesh->vertices)[i].pos, 3);

        readAccessorOr(textureAccessor, i, GLTF(mesh->vertices)[i].tex, 2, 0);
        readAccessorOr(colorAccessor, i, GLTF(mesh->vertices)[i].color, 3, 1);
        readAccessorOr(normalAccessor, i, GLTF(mesh->vertices)[i].norm, 3, 1);
    }

    if (indexAccessor) {
        mesh->indicesQuantity = indexAccessor->count;
        mesh->indices = malloc(sizeof(uint32_t) * mesh->indicesQuantity);
        for (cgltf_size i = 0; i < indexAccessor->count; i += 1) {
            mesh->indices[i] = cgltf_accessor_read_index(indexAccessor, i);
        }
    }
    else {
        mesh->indicesQuantity = mesh->verticesQuantity;
        mesh->indices = malloc(sizeof(uint32_t) * mesh->indicesQuantity);
        for (cgltf_size i = 0; i < mesh->indicesQuantity; i += 1) {
            mesh->indices[i] = i;
        }
    }
}

static size_t countMeshContainingNodes(cgltf_data *data) {
    size_t result = 0;

    for (cgltf_size i = 0; i < data->nodes_count; i += 1) {
        result += NULL != data->nodes[i].mesh;
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
        .qComponents = cgltf_num_components(output->type),
        .interpolationType = (
            channel->target_path == cgltf_animation_path_type_rotation &&
            sampler->interpolation == cgltf_interpolation_type_linear)
                ? 3 :
                sampler->interpolation
    };

    float (*val)[result.qComponents + 1] = (void *)(
        result.values = malloc(result.qFrames * (result.qComponents + 1) * sizeof(float))
    );

    assert(input->count == output->count);
    for (cgltf_size k = 0; k < input->count; k += 1) {
        cgltf_accessor_read_float(input, k, val[k], 1);
        cgltf_accessor_read_float(output, k, val[k] + 1, result.qComponents);
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
    free(objInfo->frames);

    free(objInfo->buffers);
    free(objInfo->pushConstants);

    free(objInfo);
} 

struct Materials {
	cgltf_float base_color_factor[4];
	cgltf_float metallic_factor;
	cgltf_float roughness_factor;
    cgltf_float pad0;
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
            
            materials[k][i].metallic_factor = data->materials[i].pbr_metallic_roughness.metallic_factor;
            materials[k][i].roughness_factor = data->materials[i].pbr_metallic_roughness.roughness_factor;
        }
    }
}

static void loadTextures(struct ModelInput *model, cgltf_data *obj) {
    for (size_t i = 0; i < obj->textures_count; i += 1) {
        if (obj->textures[i].image->uri == NULL) {
            model->inputTextures[i] = NULL;
        }
        else {
            model->inputTextures[i] = calloc(strlen(obj->textures[i].image->uri) + 1, sizeof(char));
            strcpy(model->inputTextures[i], obj->textures[i].image->uri);
        }
    }
}

size_t max(size_t a, size_t b) {
    return a > b ? a : b;
}

void gltfLoadModel(const char *modelPath, struct ModelInput *model, struct GraphicsSetup *graphics) {
    cgltf_options options = {};
    cgltf_data *data = NULL;

    size_t loadedMesh = 0;

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
        info->buffers[1].range = data->nodes_count * sizeof(mat4);
        info->buffers[2].range = max(data->materials_count, 1) * sizeof(struct Materials);
        model->qTextures = max(data->textures_count, 1);
        model->inputTextures = calloc(model->qTextures, sizeof(char *));

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

        for (cgltf_size i = 0; i < data->nodes_count; i += 1) {
            if (NULL != data->nodes[i].mesh) {
                assert(data->nodes[i].mesh->primitives_count == 1);
                for (cgltf_size j = 0; j < data->nodes[i].mesh->primitives_count; j += 1) {
                    loadPrimitive(&model->mesh[loadedMesh], &data->nodes[i].mesh->primitives[j]);
                }

                GLTF_PC(info->pushConstants)[loadedMesh].nodeID = i;

                loadedMesh += 1;
            }

            for (size_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j += 1) {
                loadTransformations(((mat4**)info->buffers[0].buffersMapped)[j][i], &data->nodes[i]);
                glm_mat4_identity(((mat4**)info->buffers[1].buffersMapped)[j][i]);
            }
        }

        loadMaterials(data, (void *)info->buffers[2].buffersMapped);
        loadTextures(model, data);

        info->qAnim = data->animations_count;
        info->qNodes = data->nodes_count;
        info->frames = loadAnimations(data);
    }

    cgltf_free(data);
}
