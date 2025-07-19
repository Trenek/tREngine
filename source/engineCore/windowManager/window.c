#include <stb_image.h>

#include "windowManager.h"

#include "MY_ASSERT.h"

static const uint32_t WIDTH = 800;
static const uint32_t HEIGHT = 600;

#define GET(x) ((struct windowData *)glfwGetWindowUserPointer(x))

static void framebufferResizeCallback(GLFWwindow *window, int, int) {
    GET(window)->framebufferResized = true;
}

static void stateCallback(union keyState *state, int action) {
    if (action == GLFW_REPEAT) {
        state->change = 0;
    }
    else {
        state->change = (state->press == 1 && action == GLFW_RELEASE) ||
                        (state->press == 0 && action == GLFW_PRESS);
        state->press = action == GLFW_PRESS ? 1 : 0;
    }
}

static void keyCallback(GLFWwindow *window, int key, int, int action, int) {
    stateCallback(&GET(window)->key[key], action);
}

static void mouseCallback(GLFWwindow *window, int button, int action, int) {
    stateCallback(&GET(window)->mouseButton[button], action);
}

static void scrollCallback(GLFWwindow *window, double xOffset, double yOffset) {
    double *posChange = GET(window)->scrollChange;

    posChange[0] = xOffset;
    posChange[1] = yOffset;
}

static void cursorPositionCallback(GLFWwindow *window, double xPos, double yPos) {
    double *pos = GET(window)->mousePos;

    pos[0] = xPos;
    pos[1] = yPos;
}

static void setIcon(GLFWwindow *window, const char *name) {
    GLFWimage image[1];
    image[0].pixels = stbi_load(name, &image[0].width, &image[0].height, 0, 4);

    glfwSetWindowIcon(window, 1, image);

    stbi_image_free(image[0].pixels);
}

GLFWwindow *createWindow(struct windowData *data, const char *name, const char *icon) {
    GLFWwindow *result = NULL;
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    result = glfwCreateWindow(WIDTH, HEIGHT, name, NULL, NULL);

    setIcon(result, icon);
    glfwSetWindowUserPointer(result, data);
    glfwSetFramebufferSizeCallback(result, framebufferResizeCallback);
    glfwSetKeyCallback(result, keyCallback);
    glfwSetMouseButtonCallback(result, mouseCallback);
    glfwSetScrollCallback(result, scrollCallback);
    glfwSetCursorPosCallback(result, cursorPositionCallback);

    return result;
}

void destroyWindow(GLFWwindow *window) {
    glfwDestroyWindow(window);

    glfwTerminate();
}
