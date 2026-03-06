#include <stdint.h>

#include <cglm.h>

bool isInBetween(vec2 C, vec2 B, vec2 A, vec2 D);
float getAngle(vec2 A, vec2 B, vec2 C);
bool isCounterClockwise(size_t N, vec2 poly[N]);
