#include "actualModel.h"

#include "rectangle.h"

void recLoadModel(const char *, struct ModelInput *model, struct GraphicsSetup *) {
    model->meshQuantity = 1;
    model->mesh = malloc(sizeof(struct Mesh) * model->meshQuantity);

    model->mesh->sizeOfVertex = sizeof(struct RecVertex);
    model->mesh->verticesQuantity = 4;
    model->mesh->indicesQuantity = 6;
    model->mesh->vertices = malloc(model->mesh->sizeOfVertex * model->mesh->verticesQuantity);
    model->mesh->indices = malloc(sizeof(uint32_t) * model->mesh->indicesQuantity);

    vec3 pos[4] = {
        { -1.0f, -1.0f, 0.0f },
        { 1.0f, 1.0f, 0.0f },
        { -1.0f, 1.0f, 0.0f },
        { 1.0f, -1.0f, 0.0f },
    };

    vec2 tex[4] = {
        { 0.0f, 0.0f },
        { 1.0f, 1.0f },
        { 0.0f, 1.0f },
        { 1.0f, 0.0f },
    };

    for (size_t i = 0; i < 4; i += 1) {
        glm_vec3_dup(pos[i], REC(model->mesh->vertices)[i].pos);
        glm_vec2_copy(tex[i], REC(model->mesh->vertices)[i].tex);
    }

    model->mesh->indices[0] = 0;
    model->mesh->indices[1] = 3;
    model->mesh->indices[2] = 1;
    model->mesh->indices[3] = 0;
    model->mesh->indices[4] = 1;
    model->mesh->indices[5] = 2;
}
