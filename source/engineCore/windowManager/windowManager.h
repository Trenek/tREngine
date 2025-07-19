#include <GLFW/glfw3.h>

union keyState {
    struct {
        uint8_t press : 1;
        uint8_t change : 1;
        uint8_t : 6;
    };
    uint8_t val;
};

struct windowData {
    bool framebufferResized;

    union keyState mouseButton[GLFW_MOUSE_BUTTON_LAST + 1];
    double scrollChange[2];
    union keyState key[GLFW_KEY_LAST + 1];
    unsigned char gamepad[GLFW_GAMEPAD_BUTTON_LAST + 1];
    float state[GLFW_GAMEPAD_AXIS_LAST + 1];

    double mousePos[2];
    double prevMousePos[2];
};

struct WindowManager {
    GLFWwindow *window;

    struct windowData *data;
};

enum keyStateNum {
    KEY_RELEASE = 1 << 0,
    KEY_PRESS =   1 << 1,
    KEY_REPEAT =  1 << 2,
    KEY_CHANGE =  1 << 3
};

struct WindowManager initWindowManager(const char *name, const char *icon);
void cleanupWindowControl(struct WindowManager this);

void updateWindow(struct WindowManager *windowControl);
void getDeltaPos(struct WindowManager *windowControl, double delta[2]);
uint8_t getKeyState(struct WindowManager *windowControl, int key);
uint8_t getMouseState(struct WindowManager *windowControl, int key);
double getScrollYChange(struct WindowManager *windowControl);
#define isKeyPressed(wc, key) ((KEY_PRESS | KEY_REPEAT) == getKeyState(wc, key))
#define isKeyJustPressed(wc, key) ((KEY_PRESS | KEY_CHANGE) == getKeyState(wc, key))
#define isMouseKeyPressed(wc, key) ((KEY_PRESS | KEY_REPEAT) == getMouseState(wc, key))

void setCursorMode(struct WindowManager *windowControl, int mode);

bool shouldWindowClose(struct WindowManager windowControl);
