#include <string.h>

#include "engineCore.h"

#include "renderPassCore.h"
#include "renderPassObj.h"
#include "graphicsPipelineObj.h"
#include "commandQueue.h"
#include "computePass.h"

#include "MY_ASSERT.h"

static void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, VkExtent2D swapChainExtent, uint32_t currentFrame, size_t qRenderPass, struct renderPassObj *renderPass[qRenderPass]) {
    MY_ASSERT(VK_SUCCESS == vkBeginCommandBuffer(commandBuffer, &(VkCommandBufferBeginInfo) {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = 0,
        .pInheritanceInfo = NULL,
        .pNext = NULL
    }));

    for (size_t i = 0; i < qRenderPass; i += 1) {
        VkClearValue clearValues[2] = {
            [0].color.float32 = {
                renderPass[i]->color[0],
                renderPass[i]->color[1],
                renderPass[i]->color[2],
                renderPass[i]->color[3],
            },
            [1].depthStencil = {
                .depth = 1.0f,
                .stencil = 0
            },
        }; 
        VkRect2D renderArena = {
            .offset = {
                .x = (int32_t)(renderPass[i]->coordinates[0] * swapChainExtent.width),
                .y = (int32_t)(renderPass[i]->coordinates[1] * swapChainExtent.height)
            },
            .extent = {
                .width = (uint32_t)(renderPass[i]->coordinates[2] * swapChainExtent.width),
                .height = (uint32_t)(renderPass[i]->coordinates[3] * swapChainExtent.height)
            }
        };
        VkRenderPassBeginInfo renderPassInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = renderPass[i]->renderPass->renderPass,
            .framebuffer = renderPass[i]->renderPass->swapChainFramebuffers[imageIndex],
            .renderArea = renderArena,
            .clearValueCount = 2,
            .pClearValues = clearValues
        };
        VkViewport viewport = {
            .x = renderArena.offset.x,
            .y = renderArena.offset.y,
            .width = renderArena.extent.width,
            .height = renderArena.extent.height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };

        if (renderPassInfo.renderArea.extent.width > 0 && renderPassInfo.renderArea.extent.height > 0) {
            vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
            vkCmdSetScissor(commandBuffer, 0, 1, &renderArena);

            // TODO: use vkCmdBindDescriptorSets2 in the future as it's nicer
            if (renderPass[i]->cameraBuffer.range) {
                vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderPass[i]->data[i].pipelineLayout, 0, 1, &renderPass[i]->cameraDescriptorSet[currentFrame], 0, NULL);
            }

            renderPass[i]->drawRenderPass(commandBuffer, currentFrame, renderPass[i]);

            vkCmdEndRenderPass(commandBuffer);
        }
    }

    MY_ASSERT(VK_SUCCESS == vkEndCommandBuffer(commandBuffer));
}

VkResult queueCompute(struct CommandQueue *commandQueue, struct EngineCore *vulkan, size_t qComputePass, struct ComputePass *computePass) {
    struct GraphicsSetup *graphics = &vulkan->graphics;
    int currentFrame = vulkan->currentFrame;
    VkCommandBuffer commandBuffer = commandQueue->commandBuffer[currentFrame];

    VkResult result = vkWaitForFences(graphics->device, 1, &commandQueue->inFlightFence[currentFrame], VK_TRUE, UINT64_MAX);

    VkSemaphore signalSemaphores[] = {
        commandQueue->semaphore[currentFrame],
    };

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
        .signalSemaphoreCount = sizeof(signalSemaphores) / sizeof(VkSemaphore),
        .pSignalSemaphores = signalSemaphores
    };

    if (VK_SUCCESS == result) {
        vkResetFences(graphics->device, 1, &commandQueue->inFlightFence[currentFrame]);

        vkResetCommandBuffer(commandBuffer, 0);
        MY_ASSERT(VK_SUCCESS == vkBeginCommandBuffer(commandBuffer, &(VkCommandBufferBeginInfo) {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = 0,
            .pInheritanceInfo = NULL,
            .pNext = NULL
        }));

        for (size_t i = 0; i < qComputePass; i += 1) {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePass[i].pipeline);
            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePass[i].pipelineLayout, 0, 1, &computePass[i].descriptor[currentFrame], 0, 0);
            vkCmdDispatch(commandBuffer, computePass[i].groupCountX, 1, 1);
        }

        MY_ASSERT(VK_SUCCESS == vkEndCommandBuffer(commandBuffer));

        MY_ASSERT(VK_SUCCESS == vkQueueSubmit(graphics->computeQueue, 1, &submitInfo, commandQueue->inFlightFence[currentFrame]));
    }

    return result;
}

void queueDraw(struct CommandQueue *commandQueue, struct EngineCore *vulkan, size_t qRenderPass, struct renderPassObj **renderPass, size_t qWait, VkSemaphore waitSemaphores[qWait], VkPipelineStageFlags waitStages[qWait]) {
    int currentFrame = vulkan->currentFrame;
    uint32_t imageIndex = vulkan->imageIndex;

    VkSemaphore signalSemaphores[] = {
        vulkan->graphics.swapChain.renderFinishedSemaphore[imageIndex]
    };

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = qWait,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandQueue->commandBuffer[currentFrame],
        .signalSemaphoreCount = sizeof(signalSemaphores) / sizeof(VkSemaphore),
        .pSignalSemaphores = signalSemaphores
    };

    vkResetFences(vulkan->graphics.device, 1, &commandQueue->inFlightFence[currentFrame]);

    vkResetCommandBuffer(commandQueue->commandBuffer[currentFrame], 0);
    recordCommandBuffer(commandQueue->commandBuffer[currentFrame], imageIndex, vulkan->graphics.swapChain.extent, currentFrame, qRenderPass, renderPass);

    MY_ASSERT(VK_SUCCESS == vkQueueSubmit(vulkan->graphics.graphicsQueue, 1, &submitInfo, commandQueue->inFlightFence[currentFrame]));
}

void presentFrame(struct EngineCore *vulkan, size_t qRenderPassCore, struct renderPassCore **renderPassCore, size_t , struct CommandQueue **queue) {
    switch (vkQueuePresentKHR(vulkan->graphics.presentQueue, &(VkPresentInfoKHR) {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &vulkan->graphics.swapChain.renderFinishedSemaphore[vulkan->imageIndex],
        .swapchainCount = 1,
        .pSwapchains = &vulkan->graphics.swapChain.this,
        .pImageIndices = &vulkan->imageIndex,
        .pResults = NULL
    })) {
        case VK_SUCCESS:
            vulkan->currentFrame += 1;
            vulkan->currentFrame %= MAX_FRAMES_IN_FLIGHT;

            break;
        case VK_SUBOPTIMAL_KHR:
        case VK_ERROR_OUT_OF_DATE_KHR:
            vulkan->window.data->framebufferResized = true;
            break;
        default:
            fprintf(stderr, "Oh no");
            glfwSetWindowShouldClose(vulkan->window.window, GLFW_TRUE);
            break;
    }

    if (vulkan->window.data->framebufferResized) {
        vulkan->window.data->framebufferResized = false;

        recreateSwapChain(vulkan, qRenderPassCore, renderPassCore);

        for (size_t i = 0; false; i += 1) {
            recreateCommandQueue(queue[i]);
        }
    }
}

void aquireNextImage(struct EngineCore *vulkan, VkFence *inFlightFence, VkSemaphore *semaphore) {
    vkWaitForFences(vulkan->graphics.device, 1, &inFlightFence[vulkan->currentFrame], VK_TRUE, UINT64_MAX);
    vkAcquireNextImageKHR(vulkan->graphics.device, vulkan->graphics.swapChain.this, UINT64_MAX, semaphore[vulkan->currentFrame], VK_NULL_HANDLE, &vulkan->imageIndex);
}

static void updateBuffers(size_t currentFrame, size_t qRenderPass, struct renderPassObj *renderPass[qRenderPass], VkExtent2D swapChainExtent) {
    for (uint32_t i = 0; i < qRenderPass; i += 1) {
        if (renderPass[i]->updateCameraBuffer)
            renderPass[i]->updateCameraBuffer(renderPass[i]->cameraBuffer.buffersMapped[currentFrame], (VkExtent2D) { 
                .width = renderPass[i]->coordinates[2] * swapChainExtent.width,
                .height = renderPass[i]->coordinates[3] * swapChainExtent.height,
            }, renderPass[i]->camera);
        for (uint32_t j = 0; j < renderPass[i]->qBuffersToUpdate; j += 1) {
            memcpy(
                renderPass[i]->buffersToUpdate[j].mapp[currentFrame],
                renderPass[i]->buffersToUpdate[j].buffer,
                renderPass[i]->buffersToUpdate[j].range
            );
        }
    }
}

void engineUpdate(struct EngineCore *vulkan, size_t qRenderPass, struct renderPassObj **renderPass) {
    updateWindow(&vulkan->window);
    updateDeltaTime(&vulkan->deltaTime);
    updateBuffers(vulkan->currentFrame, qRenderPass, renderPass, vulkan->graphics.swapChain.extent);
}
