#include <malloc.h>
#include <string.h>

#include "windowManager.h"

GLFWwindow *createWindow(struct windowData *data, const char *name, const char *icon);
void destroyWindow(GLFWwindow *window);

struct WindowManager initWindowManager(const char *name, const char *icon) {
    struct WindowManager result = {
        .data = calloc(1, sizeof(struct windowData))
    };

    result.window = createWindow(result.data, name, icon);

    return result;
}

void cleanupWindowControl(struct WindowManager this) {
    destroyWindow(this.window);

    free(this.data);
}

static uint8_t updateKeyState(union keyState *state) {
    uint8_t result = (state->press ? KEY_PRESS : KEY_RELEASE) |
                     (state->change ? KEY_CHANGE : KEY_REPEAT);
    state->change = 0;

    return result;
}

uint8_t getKeyState(struct WindowManager *windowControl, int key) {
    return updateKeyState(&windowControl->data->key[key]);
}

uint8_t getMouseState(struct WindowManager *windowControl, int key) {
    return updateKeyState(&windowControl->data->mouseButton[key]);
}

double getScrollYChange(struct WindowManager *windowControl) {
    return windowControl->data->scrollChange[1];
}

void updateWindow(struct WindowManager *windowControl) {
    memcpy(windowControl->data->prevMousePos, windowControl->data->mousePos, sizeof(double [2]));
    memset(windowControl->data->scrollChange, 0, sizeof(double[2]));

    glfwPollEvents();
}

void getDeltaPos(struct WindowManager *windowControl, double delta[2]) {
    delta[0] = windowControl->data->mousePos[0] - windowControl->data->prevMousePos[0];
    delta[1] = windowControl->data->mousePos[1] - windowControl->data->prevMousePos[1];
}

void setCursorMode(struct WindowManager *windowControl, int mode) {
    glfwSetInputMode(windowControl->window, GLFW_CURSOR, mode);
}

bool shouldWindowClose(struct WindowManager windowControl) {
    if (GLFW_PRESS == glfwGetKey(windowControl.window, GLFW_KEY_END)) {
        glfwSetWindowShouldClose(windowControl.window, GLFW_TRUE);
    }

    return glfwWindowShouldClose(windowControl.window);
}
