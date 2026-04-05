#include "engineCore.h"
#include "renderPassCore.h"

void recreateSwapChain(struct EngineCore *engine, size_t qRenderPassCore, struct renderPassCore **renderPassCore) {
    int width = 0;
    int height = 0;

    glfwGetFramebufferSize(engine->window.window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(engine->window.window, &width, &height);
        glfwWaitEvents();
    }
    
    vkDeviceWaitIdle(engine->graphics.device);

    recreateSwapChainGraphics(engine->window.window, &engine->graphics);
    for (size_t i = 0; i < qRenderPassCore; i += 1) {
        recreateRenderPassCore(renderPassCore[i], &engine->graphics);
    }
}

struct EngineCore setup(const char *name, const char *icon) {
    struct EngineCore engine = {
        .window = initWindowManager(name, icon),
        .deltaTime = initDeltaTimeManager(),
        .resource = {}
    };

    engine.graphics = setupGraphics(engine.window.window);

    return engine;
}

void cleanup(struct EngineCore engine) {
    vkDeviceWaitIdle(engine.graphics.device);
    cleanupResourcesOrg(&engine.resource);

    cleanupGraphics(engine.graphics);
    cleanupWindowControl(engine.window);
}
