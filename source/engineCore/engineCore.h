#ifndef ENGINE_CORE_H
#define ENGINE_CORE_H

#include "windowManager.h"
#include "deltaTime.h"
#include "soundManager.h"
#include "resourceManager.h"

#include "graphicsSetup.h"

struct renderPassCore;
struct EngineCore {
    struct WindowManager window;
    struct DeltaTimeManager deltaTime;
    struct SoundManager soundManager;
    struct ResourceManager resource;

    struct GraphicsSetup graphics;
};

struct EngineCore setup(const char *name, const char *icon);
void recreateSwapChain(struct EngineCore *engine, uint16_t qRenderPassCore, struct renderPassCore **renderPassCore);
void cleanup(struct EngineCore engine);

struct renderPassObj;
void drawFrame(struct EngineCore *engine, uint16_t qRenderPass, struct renderPassObj **renderPass, uint16_t qRenderPassCore, struct renderPassCore **renderPassCore);

#endif
