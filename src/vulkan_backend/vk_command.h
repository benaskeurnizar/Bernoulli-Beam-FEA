#pragma once

#include "vk_types.h"
#include "vk_device.h"

typedef struct {
  VkCommandPool pool;
  VkCommandBuffer* buffers;   // array, size MAX_FRAMES_IN_FLIGHT
} CommandCtx;

void createCommandCtx(CommandCtx* commandCtx,VkDeviceCtx* deviceCtx,VkSurfaceKHR surface);
void cleanupCommandCtx(CommandCtx* commandCtx,VkDeviceCtx* deviceCtx);
