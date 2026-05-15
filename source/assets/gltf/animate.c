#include <string.h>
#include <assert.h>
#include <cgltf.h>

#include "model.h"
#include "entity.h"
#include "gltf.h"
#include "gltfBuilder.h"

static void stepInterpolation(float, float, size_t size, float v0[size], float[size], float, float *out) {
    memcpy(out, v0, sizeof(float[size]));
}

static void linearInterpolation(float t0, float t1, size_t size, float v0[size], float v1[size], float time, float *out) {
    float ratio = (time - t0) / (t1 - t0);

    for (size_t i = 0; i < size; i += 1) {
        out[i] = glm_lerp(v0[i], v1[i], ratio);
    }
}

static void cubicInterpolation(float, float, size_t size, float v0[size], float[size], float, float *out) {
    assert(false);
    memcpy(out, v0, sizeof(float[size]));
}

static void sphericalLinearInterpolation(float t0, float t1, size_t size, float v0[size], float v1[size], float time, float *out) {
    vec4 prevQuat; {
        prevQuat[0] = v0[0];
        prevQuat[1] = v0[1];
        prevQuat[2] = v0[2];
        prevQuat[3] = v0[3];

        glm_quat_normalize(prevQuat);
    }
    vec4 nextQuat; {
        nextQuat[0] = v1[0];
        nextQuat[1] = v1[1];
        nextQuat[2] = v1[2];
        nextQuat[3] = v1[3];

        glm_quat_normalize(nextQuat);
    }
    float dotProduct = glm_quat_dot(prevQuat, nextQuat);
    float ratio = (time - t0) / (t1 - t0);
    assert(ratio >= 0 && ratio <= 1);

    if (dotProduct < 0.0) {
        glm_vec4_scale(nextQuat, -1, nextQuat);
        dotProduct = -dotProduct;
    }

    if (dotProduct > 1 - 10e-6) {
        linearInterpolation(t0, t1, size, v0, v1, time, out);
    }
    else {
        float theta_0 = acos(dotProduct);
        float theta = ratio * theta_0;

        float scaleNextQuat = sin(theta) / sin(theta_0);
        float scalePreviousQuat = cos(theta) - dotProduct * scaleNextQuat;

        glm_vec4_scale(prevQuat, scalePreviousQuat, prevQuat);
        glm_vec4_scale(nextQuat, scaleNextQuat, nextQuat);
        glm_vec4_add(prevQuat, nextQuat, out);
    }
}

static void interpolateData(struct Frames *frames, float deltaTime, float *data) {
    size_t qData = frames->qFrames;
    float (*values)[frames->qComponents + 1] = (void *)frames->values;
    float maxTime = values[qData - 1][0];
    float time = maxTime == 0.0f ? 0.0f : fmodf(deltaTime, maxTime);

    void (*interpolation[])(float t0, float t1, size_t size, float v0[size], float v1[size], float time, float *out) = {
        linearInterpolation,
        stepInterpolation,
        cubicInterpolation,
        sphericalLinearInterpolation,
    };

    if (time < values[0][0]) {
        memcpy(data, values[0] + 1, sizeof(float[frames->qComponents]));
    }
    else {
        size_t i = 0; {
            while (values[i + 1][0] < time) i += 1;
        }

        interpolation[frames->interpolationType](values[i][0], values[i + 1][0], frames->qComponents, &values[i][1], &values[i + 1][1], time, data);
    }
}

static void calculateJointData(struct Frames data[ANIM_PATH_TYPE_MAX_ENUM], struct AnimationData *out, vec3 defTra, vec3 defSca, vec4 defRot, float deltaTime) {
    glm_mat4_identity(out->animation);

    if (data[cgltf_animation_path_type_translation].qFrames) {
        vec4 interpolatedData; {
            interpolateData(&data[cgltf_animation_path_type_translation], deltaTime, interpolatedData);
        }
        glm_translate(out->animation, interpolatedData);
    }
    else {
        glm_translate(out->animation, defTra);
    }

    if (data[cgltf_animation_path_type_rotation].qFrames) {
        vec4 interpolatedData; {
            interpolateData(&data[cgltf_animation_path_type_rotation], deltaTime, interpolatedData);
        }
        glm_quat_rotate(out->animation, interpolatedData, out->animation);
    }
    else {
        glm_quat_rotate(out->animation, defRot, out->animation);
    }

    if (data[cgltf_animation_path_type_scale].qFrames) {
        vec4 interpolatedData; {
            interpolateData(&data[cgltf_animation_path_type_scale], deltaTime, interpolatedData);
        }
        glm_scale(out->animation, interpolatedData);
    }
    else {
        glm_scale(out->animation, defSca);
    }

    if (data[cgltf_animation_path_type_weights].qFrames) {
        vec4 interpolatedData = { 0, 0, 0, 0 }; {
            interpolateData(&data[cgltf_animation_path_type_weights], deltaTime, interpolatedData);
        }

        out->weight1 = interpolatedData[0];
        out->weight2 = interpolatedData[1];
        out->weight3 = interpolatedData[2];
        out->weight4 = interpolatedData[3];
    }
}

void loadAccessorsTransformations(struct Skin *skins, float deltaTime, struct Frames (*frames)[ANIM_PATH_TYPE_MAX_ENUM], struct NodeData *nodes, int nodeID, mat4 out) {
    struct AnimationData data;

    int jointID = skins->jointID[nodeID];
    int fatherID = jointID == -1 ? -1 : skins->joint[jointID].father;
    int fatherNodeID = -1;

    while (fatherID != -1) {
        fatherNodeID = skins->joint[fatherID].nodeID;

        calculateJointData(frames[fatherNodeID],
            &data,
            nodes[fatherNodeID].translation,
            nodes[fatherNodeID].scale,
            nodes[fatherNodeID].rotation,
            deltaTime
        );

        glm_mat4_mul(data.animation, out, out);

        jointID = skins->jointID[fatherNodeID];
        fatherID = jointID == -1 ? -1 : skins->joint[jointID].father;
    }
}

void animate(struct Entity *entity, struct Model *model, size_t animID, float deltaTime) {
    struct GltfModelInfo *info = model->info;

    struct Frames (*animFrames)[info->qNodes][ANIM_PATH_TYPE_MAX_ENUM] = (void *)info->frames;
    struct AnimationData *data = entity->buffer[1];
    struct Skin *skins = info->skin;

    mat4 ancestors;

    if (animID < info->qAnim) for (size_t i = 0; i < info->qNodes; i += 1) {
        calculateJointData(animFrames[animID][i], &data[i],
            info->nodes[i].translation,
            info->nodes[i].scale,
            info->nodes[i].rotation,
            deltaTime
        );

        data[i].jointToNodeID = 0;

        if (info->qSkin) {
            glm_mat4_identity(ancestors);
            loadAccessorsTransformations(skins, deltaTime, animFrames[animID], info->nodes, i, ancestors);

            glm_mat4_mul(ancestors, data[i].animation, data[i].animation);
            if (skins->jointID[i] != -1) {
                glm_mat4_mul(
                    data[i].animation, 
                    skins->joint[skins->jointID[i]].inverseMatrix, 
                    data[i].animation
                );
            }

            if (i < skins->qJoint) {
                data[i].jointToNodeID = skins->joint[i].nodeID;
            }
        }
    }
    else for (size_t i = 0; i < info->qNodes; i += 1) {
        glm_mat4_identity(data[i].animation);
    }
}
