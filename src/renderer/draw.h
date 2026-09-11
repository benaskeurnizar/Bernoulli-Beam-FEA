#pragma once

#include "renderer.h"
#include "../vulkan_backend/vk_pipeline.h"
#include "../vulkan_backend/vk_device.h"
#include "../vulkan_backend/vk_swapChain.h"
#include "../platform/window.h"

void drawFrame(Renderer* renderer,Window* window,World* world,FeaModel* model);

void recreateSwapChain(VkSwapchainCtx* swapChainCtx,VkPipelineCtx* pipelineCtx,VkDeviceCtx* deviceCtx,Window* window,VkSurfaceKHR surface);
