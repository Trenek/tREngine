#include <math.h>
#include <stddef.h>

#include "isClockWise.h"

static float vec2Angle(vec2 a, vec2 b) {
    vec2 sub = {
        a[0] - b[0],
        a[1] - b[1]
    };
    float angle = atan2(sub[1], sub[0]);

    return angle + (angle < 0) * 2 * M_PI;
}

bool isInBetween(vec2 C, vec2 B, vec2 A, vec2 D) {
    float ba = vec2Angle(A, B);
    float bc = vec2Angle(C, B);
    float bd = vec2Angle(D, B);

    return (
        bc > ba ? bd > ba && bc > bd :
        bc < ba ? !(bd > bc && ba > bd) :
                  bd == ba && bd == bc
    );
}

float getAngle(vec2 A, vec2 B, vec2 C) {
    float ba = vec2Angle(A, B);
    float bc = vec2Angle(C, B);

    float result = ba - bc;

    if (result < 0) result += 2 * M_PI;

    return result;
}

bool isCounterClockwise(size_t N, vec2 poly[N]) {
    double angle = 0;

    for (size_t i = 0; i < N; i += 1) {
        angle += getAngle(poly[(i - 1 + N) % N], poly[i], poly[(i + 1) % N]);
    }

    return fabs(angle - M_PI * (N - 2)) < 10e-5;
}
