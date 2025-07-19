#include <cglm.h>

#include "camera.h"
#include "windowManager.h"

int sgn(float x) {
    return x > 0 ? 1 : x < 0 ? -1 : 0;
}

void moveThirdPersonCamera(struct WindowManager *windowControl, union camera *camera, float deltaTime) {
    float x = camera->tP.relativePos[0] - camera->tP.center[0];
    float y = camera->tP.relativePos[1] - camera->tP.center[1];
    float z = camera->tP.relativePos[2] - camera->tP.center[2];

    float r = sqrt(x * x + y * y + z * z);
    float theta = acos(z / r);
    float phi = sgn(y) * acos(x / sqrt(x * x + y * y));

    float speed = deltaTime * (1 + 3 * isKeyPressed(windowControl, GLFW_KEY_LEFT_SHIFT));
    bool isMouseUsed = isMouseKeyPressed(windowControl, GLFW_MOUSE_BUTTON_LEFT);
    float mouseSpeed = isMouseUsed * speed;

    double deltaPos[2]; {
        getDeltaPos(windowControl, deltaPos);
    }

    setCursorMode(windowControl, isMouseUsed ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

    r     -= speed * isKeyPressed(windowControl, GLFW_KEY_UP);
    r     += speed * isKeyPressed(windowControl, GLFW_KEY_DOWN);
    theta -= speed * isKeyPressed(windowControl, GLFW_KEY_W);
    theta += speed * isKeyPressed(windowControl, GLFW_KEY_S);
    phi   -= speed * isKeyPressed(windowControl, GLFW_KEY_A);
    phi   += speed * isKeyPressed(windowControl, GLFW_KEY_D);

    r     -= speed * getScrollYChange(windowControl);
    theta -= mouseSpeed * deltaPos[1];
    phi   -= mouseSpeed * deltaPos[0];

    camera->tP.relativePos[0] = camera->tP.center[0] + r * sin(theta) * cos(phi);
    camera->tP.relativePos[1] = camera->tP.center[1] + r * sin(theta) * sin(phi);
    camera->tP.relativePos[2] = camera->tP.center[2] + r * cos(theta);
}
