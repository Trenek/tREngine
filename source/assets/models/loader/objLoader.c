#include <string.h>
#include <fast_obj.h>

#include "actualModel.h"

#include "Vertex.h"

#define OBJ(X) ((struct ObjVertex *)X)

size_t countV(fastObjMesh *obj, fastObjGroup grp) {
    size_t idx = 0;

    for (size_t i = 0; i < grp.face_count; i += 1) {
        idx += (obj->face_vertices + grp.face_offset)[i];
    }

    return idx;
}

size_t countI(fastObjMesh *obj, fastObjGroup grp) {
    size_t idx = 0;

    for (size_t i = 0; i < grp.face_count; i += 1) {
        idx += 3 * ((obj->face_vertices + grp.face_offset)[i] - 2);
    }

    return idx;
}

void loadIndex(uint32_t (*index)[3], size_t qVertex, size_t loadedVertex) {
    for (size_t j = 0; j < qVertex - 2; j += 1) {
        index[j][0] = loadedVertex + 0;
        index[j][1] = loadedVertex + j + 1;
        index[j][2] = loadedVertex + j + 2;
    }
}

void loadVertex(struct ObjVertex *v, fastObjMesh *obj, fastObjIndex *objIndex, size_t qVertex) {
    for (size_t j = 0; j < qVertex; j += 1) {
        memcpy(v[j].geo, obj->positions + 3 * objIndex[j].p, sizeof(float[3]));
        memcpy(v[j].norm, obj->normals + 3 * objIndex[j].n, sizeof(float[3]));
        memcpy(v[j].tex, obj->texcoords + 3 * objIndex[j].t, sizeof(float[3]));
    }
}

void loadGroup(struct MeshInput *mesh, fastObjMesh *obj, fastObjGroup grp) {
    size_t idv = 0;
    size_t idi = 0;

    uint32_t *qVertex = obj->face_vertices + grp.face_offset;
    fastObjIndex *objIndex = obj->indices + grp.index_offset;

    mesh->sizeOfVertex = sizeof(struct ObjVertex);
    mesh->verticesQuantity = countV(obj, grp);
    mesh->indicesQuantity = countI(obj, grp);
    mesh->vertices = malloc(mesh->sizeOfVertex * mesh->verticesQuantity);
    mesh->indices = malloc(sizeof(uint32_t) * mesh->indicesQuantity);

    for (size_t i = 0; i < grp.face_count; i += 1) {
        loadIndex((void *)(mesh->indices + idi), qVertex[i], idv);
        loadVertex(OBJ(mesh->vertices) + idv, obj, objIndex + idv, qVertex[i]);

        idi += 3 * (qVertex[i] - 2);
        idv += qVertex[i];
    }
}

void objLoadModel(const char *objectPath, struct ModelInput *model, VkDevice, VkPhysicalDevice, VkSurfaceKHR) {
    fastObjMesh *obj = fast_obj_read(objectPath);

    model->meshQuantity = obj->group_count;
    model->mesh = malloc(sizeof(struct Mesh) * model->meshQuantity);

    for (size_t i = 0; i < obj->group_count; i += 1) {
        loadGroup(&model->mesh[i], obj, obj->groups[i]);
    }

    fast_obj_destroy(obj);
}
