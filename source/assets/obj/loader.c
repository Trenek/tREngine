#include <string.h>
#include <fast_obj.h>

#include "graphicsSetup.h"
#include "texture.h"
#include "model.h"

#include "bufferObj.h"
#include "obj.h"

static size_t countV(fastObjMesh *obj, fastObjGroup grp) {
    size_t idx = 0;

    for (size_t i = 0; i < grp.face_count; i += 1) {
        idx += (obj->face_vertices + grp.face_offset)[i];
    }

    return idx;
}

static size_t countI(fastObjMesh *obj, fastObjGroup grp) {
    size_t idx = 0;

    for (size_t i = 0; i < grp.face_count; i += 1) {
        idx += 3 * ((obj->face_vertices + grp.face_offset)[i] - 2);
    }

    return idx;
}

static void loadIndex(uint32_t (*index)[3], size_t qVertex, size_t loadedVertex) {
    for (size_t j = 0; j < qVertex - 2; j += 1) {
        index[j][0] = loadedVertex + 0;
        index[j][1] = loadedVertex + j + 1;
        index[j][2] = loadedVertex + j + 2;
    }
}

static void loadVertex(struct ObjVertex *v, fastObjMesh *obj, fastObjIndex *objIndex, size_t qVertex, size_t materialIndex) {
    float (*positions)[3] = (void *)obj->positions;
    float (*normals)[3] = (void *)obj->normals;
    float (*texcoords)[2] = (void *)obj->texcoords;

    for (size_t i = 0; i < qVertex; i += 1) {
        memcpy(v[i].geo, positions + objIndex[i].p, sizeof(float[3]));
        memcpy(v[i].norm, normals + objIndex[i].n, sizeof(float[3]));

        v[i].tex[0] = texcoords[objIndex[i].t][0];
        v[i].tex[1] = 1.0f - texcoords[objIndex[i].t][1];
        v[i].material = materialIndex;
    }
}

static void loadGroup(struct MeshInput *mesh, fastObjMesh *obj, fastObjGroup grp) {
    size_t idv = 0;
    size_t idi = 0;

    uint32_t *qVertex = obj->face_vertices + grp.face_offset;
    uint32_t *materialIndex = obj->face_materials + grp.face_offset;
    fastObjIndex *objIndex = obj->indices + grp.index_offset;

    mesh->sizeOfVertex = sizeof(struct ObjVertex);
    mesh->verticesQuantity = countV(obj, grp);
    mesh->indicesQuantity = countI(obj, grp);
    mesh->vertices = malloc(mesh->sizeOfVertex * mesh->verticesQuantity);
    mesh->indices = malloc(sizeof(uint32_t) * mesh->indicesQuantity);

    for (size_t i = 0; i < grp.face_count; i += 1) {
        loadIndex((void *)(mesh->indices + idi), qVertex[i], idv);
        loadVertex(OBJ(mesh->vertices) + idv, obj, objIndex + idv, qVertex[i], materialIndex[i]);

        idi += 3 * (qVertex[i] - 2);
        idv += qVertex[i];
    }
}

static void loadObjTextures(struct ModelInput *model, fastObjMesh *obj) {
    for (size_t i = 0; i < obj->texture_count; i += 1) {
        model->texture[i].mode = FROM_OTHER;

        if (obj->textures[i].path == NULL) {
            model->texture[i].data = NULL;
        }
        else {
            model->texture[i].data = strdup(obj->textures[i].path);
        }
    }
}

struct Materials {
    float                       Ka[4];  /* Ambient */
    float                       Kd[4];  /* Diffuse */
    float                       Ks[4];  /* Specular */
    float                       Ke[4];  /* Emission */
    float                       Kt[4];  /* Transmittance */
    float                       Tf[4];  /* Transmission filter */
    float                       Ns;     /* Shininess */
    float                       Ni;     /* Index of refraction */
    float                       d;      /* Disolve (alpha) */
    int                         illum;  /* Illumination model */

    /* Set for materials that don't come from the associated mtllib */
    int                         fallback;

    /* Texture map indices in fastObjMesh textures array */
    unsigned int                map_ka;
    unsigned int                map_kd;
    unsigned int                map_ks;
    unsigned int                map_ke;
    unsigned int                map_kt;
    unsigned int                map_ns;
    unsigned int                map_ni;
    unsigned int                map_d;
    unsigned int                map_bump;
    int pad0;
    int pad1;
};

static struct Materials *loadMaterials(fastObjMesh *obj, size_t materialCount) {
    struct Materials *materials = malloc(sizeof(struct Materials) * materialCount);
    vec3 one = { 1.0f, 1.0f, 1.0f };

    if (0 == obj->material_count) {
        memset(materials, 0, sizeof(struct Materials));

        glm_vec3_dup(one, materials->Ka);
        glm_vec3_dup(one, materials->Kd);
    }
    else for (size_t i = 0; i < obj->material_count; i += 1) {
        memcpy(materials[i].Ka, obj->materials[i].Ka, sizeof(float[3]));
        memcpy(materials[i].Kd, obj->materials[i].Kd, sizeof(float[3]));
        memcpy(materials[i].Ks, obj->materials[i].Ks, sizeof(float[3]));
        memcpy(materials[i].Ke, obj->materials[i].Ke, sizeof(float[3]));
        memcpy(materials[i].Kt, obj->materials[i].Kt, sizeof(float[3]));
        memcpy(materials[i].Tf, obj->materials[i].Tf, sizeof(float[3]));

        materials[i].Ns = obj->materials[i].Ns;
        materials[i].Ni = obj->materials[i].Ni;
        materials[i].d = obj->materials[i].d;
        materials[i].illum = obj->materials[i].illum;
        materials[i].fallback = obj->materials[i].fallback;

        materials[i].map_ka = obj->materials[i].map_Ka;
        materials[i].map_kd = obj->materials[i].map_Kd;
        materials[i].map_ks = obj->materials[i].map_Ks;
        materials[i].map_ke = obj->materials[i].map_Ke;
        materials[i].map_kt = obj->materials[i].map_Kt;
        materials[i].map_ns = obj->materials[i].map_Ns;
        materials[i].map_ni = obj->materials[i].map_Ni;
        materials[i].map_d = obj->materials[i].map_d;
        materials[i].map_bump = obj->materials[i].map_bump;
    }

    return materials;
}

static void cleanupObjModelInfo(void *objInfoPtr) {
    struct ObjModelInfo *objInfo = objInfoPtr;

    destroyBufferObj(objInfo->buffer);

    free(objInfo);
} 

static size_t max(size_t a, size_t b) {
    return a > b ? a : b;
}

void objLoadModel(const char *objectPath, struct ModelInput *model, struct GraphicsSetup *graphics) {
    fastObjMesh *obj = fast_obj_read(objectPath);

    struct ObjModelInfo *info = model->info = malloc(sizeof(struct ObjModelInfo));

    model->cleanup = cleanupObjModelInfo;
    model->meshQuantity = obj->group_count;
    model->mesh = malloc(sizeof(struct MeshInput) * model->meshQuantity);

    info->device = graphics->device;
    model->qTexture = obj->texture_count;
    model->texture = malloc(sizeof(struct TextureData) * obj->texture_count);

    for (size_t i = 0; i < obj->group_count; i += 1) {
        loadGroup(&model->mesh[i], obj, obj->groups[i]);
    }
    loadObjTextures(model, obj);

    const size_t materialCount = max(1, obj->material_count);
    void * const materials = loadMaterials(obj, materialCount);

    // materials
    struct BufferObj *srcBuffer = createBufferObj((struct BufferBuilder) {
        .bufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .memoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        .size = sizeof(struct Materials) * max(1, obj->material_count),
        .repetitions = 1,
    }, graphics);

    void *map = NULL;
    vkMapMemory(graphics->device, srcBuffer->memory, 0, srcBuffer->range, 0, &map);
    memcpy(map, materials, srcBuffer->range);
    vkUnmapMemory(graphics->device, srcBuffer->memory);

    info->buffer = createBufferObj((struct BufferBuilder) {
        .bufferUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                       VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        .memoryProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        .size = sizeof(struct Materials) * max(1, obj->material_count),
        .repetitions = 1,
    }, graphics);

    copyBufferObj(info->buffer, srcBuffer, 1, (VkBufferCopy[]) {
        {
            .size = srcBuffer->range,
            .srcOffset = 0,
            .dstOffset = 0,
        }
    }, graphics);
    
    destroyBufferObj(srcBuffer);
    free(materials);
    fast_obj_destroy(obj);
}
