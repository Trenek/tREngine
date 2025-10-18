#include <string.h>
#include <assert.h>

#include "entity.h"
#include "actualModel.h"

static void stepInterpolation(struct timeFrame *frames, size_t pID, float, float *out) {
    memcpy(out, frames->data[pID].values, sizeof(float[frames->qValues]));
}

static void linearInterpolation(struct timeFrame *frames, size_t pID, float time, float *out) {
    float ratio = (
        (time                       - frames->data[pID].time) / 
        (frames->data[pID + 1].time - frames->data[pID].time)
    );

    for (size_t i = 0; i < frames->qValues; i += 1) {
        out[i] = glm_lerp(frames->data[pID].values[i], frames->data[pID + 1].values[i], ratio);
    }
}

static void cubicInterpolation(struct timeFrame *frames, size_t pID, float, float *out) {
    assert(false);
    memcpy(out, frames->data[pID].values, sizeof(float[frames->qValues]));
}

static void sphericalLinearInterpolation(struct timeFrame *frames, size_t pID, float time, float *out) {
    vec4 prevQuat; {
        glm_vec4_dup(frames->data[pID].values, prevQuat);
        glm_quat_normalize(prevQuat);
    }
    vec4 nextQuat; {
        glm_vec4_dup(frames->data[pID + 1].values, nextQuat);
        glm_quat_normalize(nextQuat);
    }
    float dotProduct = glm_quat_dot(prevQuat, nextQuat);
    float ratio = (
        (time                       - frames->data[pID].time) / 
        (frames->data[pID + 1].time - frames->data[pID].time)
    );
    assert(ratio >= 0 && ratio <= 1);

    if (dotProduct < 0.0) {
        glm_vec4_scale(nextQuat, -1, nextQuat);
        dotProduct = -dotProduct;
    }

    if (dotProduct > 1 - 10e-6) {
        linearInterpolation(frames, pID, time, out);
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

static void interpolateData(struct timeFrame *frames, float deltaTime, float *data) {
    size_t qData = frames->qData;
    float maxTime = frames->data[qData - 1].time;
    float time = maxTime == 0.0f ? 0.0f : fmodf(deltaTime, maxTime);

    void (*interpolation[])(struct timeFrame *, size_t, float, vec4) = {
        linearInterpolation,
        stepInterpolation,
        cubicInterpolation,
        sphericalLinearInterpolation,
    };

    if (time < frames->data[0].time) {
        memcpy(data, frames->data[0].values, sizeof(float[frames->qValues]));
    }
    else {
        size_t i = 0; {
            while (frames->data[i + 1].time < time) i += 1;
        }

        interpolation[frames->interpolationType](frames, i, time, data);
    }
}

static void calculateJointData(size_t index, struct skinData *skinData, struct timeFrame (*data)[4], mat4 out, float deltaTime) {
    if (skinData != NULL && skinData->jointData[index].father >= 0) {
        calculateJointData(skinData->jointData[index].father, skinData, data, out, deltaTime);
    }

    if (data[index][0].qData) {
        vec4 interpolatedData; {
            interpolateData(&data[index][0], deltaTime, interpolatedData);
        }

        glm_translate(out, interpolatedData);
    }

    if (data[index][1].qData) {
        vec4 interpolatedData; {
            interpolateData(&data[index][1], deltaTime, interpolatedData);
        }

        glm_quat_rotate(out, interpolatedData, out);
    }

    if (data[index][2].qData) {
        vec4 interpolatedData; {
            interpolateData(&data[index][2], deltaTime, interpolatedData);
        }

        glm_scale(out, interpolatedData);
    }
}

void animate(struct Entity *model, struct actualModel *actualModel, size_t animID, float deltaTime) {
    struct timeFrame (*data)[4] = &actualModel->anim[animID];
    struct skinData *skinData = actualModel->skin;

    mat4 *mat = model->buffer[2];

    for (uint32_t i = 0; i < actualModel->qNode; i += 1) {
        glm_mat4_identity(mat[i]);
        
        if (actualModel->anim) {
            calculateJointData(i, skinData, data, mat[i], deltaTime);
            if (skinData != NULL && skinData->jointID[i] >= 0) {
                glm_mat4_mul(mat[i], skinData->jointData[skinData->jointID[i]].inverseMatrix, mat[i]);
            }
        }
    }
}
