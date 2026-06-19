#ifndef ENGINE_CORE_H
#define ENGINE_CORE_H

#include "windowManager.h"
#include "deltaTime.h"
#include "resourceManager.h"

#include "graphicsSetup.h"

struct renderPassCore;
struct EngineCore {
    struct WindowManager window;
    struct DeltaTimeManager deltaTime;
    struct ResourceManager resource;

    struct GraphicsSetup graphics;
    size_t currentFrame;
    uint32_t imageIndex;
};

struct EngineCore setup(const char *name, const char *icon);
void recreateSwapChain(struct EngineCore *engine, size_t qRenderPassCore, struct renderPassCore **renderPassCore);
void cleanup(struct EngineCore engine);

struct renderPassObj;
struct ComputePass;
struct CommandQueue;

void engineUpdate(struct EngineCore *vulkan, size_t qRenderPass, struct renderPassObj **renderPass);
void aquireNextImage(struct EngineCore *vulkan, VkFence *inFlightFence, VkSemaphore *semaphore);
VkResult queueCompute(struct CommandQueue *commandQueue, struct EngineCore *vulkan, size_t qComputePass, struct ComputePass *computePass);
VkResult queueComputeQP(struct CommandQueue *commandQueue, struct EngineCore *vulkan, size_t qComputePass, struct ComputePass *computePass, VkQueryPool qp);
void queueDraw(struct CommandQueue *commandQueue, struct EngineCore *vulkan, size_t qRenderPass, struct renderPassObj **renderPass, size_t qWait, VkSemaphore waitSem[qWait], VkPipelineStageFlags waitStage[qWait]);

void presentFrame(struct EngineCore *vulkan, size_t qRenderPassCore, struct renderPassCore **renderPassCore, size_t qQueue, struct CommandQueue **queue);

#endif
