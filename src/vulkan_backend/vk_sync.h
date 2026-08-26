#pragma once

#include "vk_types.h"
#include "vk_swapChain.h"
#include "vk_device.h"

typedef struct{
  VkSemaphore* image_available;
  uint32_t availableCount;
  VkSemaphore* render_finished;
  uint32_t finishedCount;
  VkFence* in_flight;
  uint32_t in_flightCount;
}VkSyncCtx;


void createSyncObjects(VkSyncCtx* syncCtx,VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx);

void cleanupSyncObjects(VkSyncCtx* syncCtx,VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx);
