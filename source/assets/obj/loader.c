#include <string.h>
#include <fast_obj.h>

#include "actualModel.h"
#include "graphicsSetup.h"

#include "bufferOperations.h"
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

static void loadTextures(struct ModelInput *model, fastObjMesh *obj) {
    for (size_t i = 0; i < obj->texture_count; i += 1) {
        if (obj->textures[i].path == NULL) {
            model->inputTextures[i] = NULL;
        }
        else {
            model->inputTextures[i] = calloc(strlen(obj->textures[i].path) + 1, sizeof(char));
            strcpy(model->inputTextures[i], obj->textures[i].path);
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

static void loadMaterials(fastObjMesh *obj, struct Materials *materials[MAX_FRAMES_IN_FLIGHT]) {
    vec3 one = { 1.0f, 1.0f, 1.0f };

    if (0 == obj->material_count) {
        for (uint32_t k = 0; k < MAX_FRAMES_IN_FLIGHT; k += 1) {
            memset(materials[k], 0, sizeof(struct Materials));

            glm_vec3_dup(one, materials[k]->Ka);
            glm_vec3_dup(one, materials[k]->Kd);
        }
    }
    else for (size_t i = 0; i < obj->material_count; i += 1) {
        for (uint32_t k = 0; k < MAX_FRAMES_IN_FLIGHT; k += 1) {
            memcpy(materials[k][i].Ka, obj->materials[i].Ka, sizeof(float[3]));
            memcpy(materials[k][i].Kd, obj->materials[i].Kd, sizeof(float[3]));
            memcpy(materials[k][i].Ks, obj->materials[i].Ks, sizeof(float[3]));
            memcpy(materials[k][i].Ke, obj->materials[i].Ke, sizeof(float[3]));
            memcpy(materials[k][i].Kt, obj->materials[i].Kt, sizeof(float[3]));
            memcpy(materials[k][i].Tf, obj->materials[i].Tf, sizeof(float[3]));

            materials[k][i].Ns = obj->materials[i].Ns;
            materials[k][i].Ni = obj->materials[i].Ni;
            materials[k][i].d = obj->materials[i].d;
            materials[k][i].illum = obj->materials[i].illum;
            materials[k][i].fallback = obj->materials[i].fallback;

            materials[k][i].map_ka = obj->materials[i].map_Ka;
            materials[k][i].map_kd = obj->materials[i].map_Kd;
            materials[k][i].map_ks = obj->materials[i].map_Ks;
            materials[k][i].map_ke = obj->materials[i].map_Ke;
            materials[k][i].map_kt = obj->materials[i].map_Kt;
            materials[k][i].map_ns = obj->materials[i].map_Ns;
            materials[k][i].map_ni = obj->materials[i].map_Ni;
            materials[k][i].map_d = obj->materials[i].map_d;
            materials[k][i].map_bump = obj->materials[i].map_bump;
        }
    }
}

static void cleanupObjModelInfo(void *objInfoPtr) {
    struct ObjModelInfo *objInfo = objInfoPtr;

    if (NULL != objInfo->buffers) {
        destroyBuffer(objInfo->device, objInfo->buffers->buffers, objInfo->buffers->buffersMemory);
    }

    free(objInfo->buffers);
    free(objInfo->pushConstants);

    free(objInfo);
} 

void objLoadModel(const char *objectPath, struct ModelInput *model, struct GraphicsSetup *graphics) {
    fastObjMesh *obj = fast_obj_read(objectPath);

    struct ObjModelInfo *info = model->info = malloc(sizeof(struct ObjModelInfo));

    model->cleanup = cleanupObjModelInfo;
    model->meshQuantity = obj->group_count;
    model->mesh = malloc(sizeof(struct Mesh) * model->meshQuantity);

    info->device = graphics->device;
    info->buffers = malloc(sizeof(struct buffer));
    info->buffers[0].range =
        sizeof(struct Materials) *
        (obj->material_count == 0 ? 1 : obj->material_count);
    model->qTextures = obj->texture_count;
    model->inputTextures = malloc(sizeof(const char *) * obj->texture_count);

    for (size_t i = 0; i < obj->group_count; i += 1) {
        loadGroup(&model->mesh[i], obj, obj->groups[i]);
    }
    loadTextures(model, obj);

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

    loadMaterials(obj, (void *)info->buffers[0].buffersMapped);
    
    fast_obj_destroy(obj);
}
