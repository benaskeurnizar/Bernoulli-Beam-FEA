#pragma once

#ifdef __cplusplus
extern "C"{
  
#endif

#include "vk_types.h"
#include "vk_device.h"

  typedef struct {
  VkCommandPool pool;
  VkCommandBuffer* buffers;   // array, size MAX_FRAMES_IN_FLIGHT
} CommandCtx;

  void createCommandCtx(CommandCtx* commandCtx,VkDeviceCtx* deviceCtx,VkSurfaceKHR surface);
  void cleanupCommandCtx(CommandCtx* commandCtx,VkDeviceCtx* deviceCtx);


  VkCommandBuffer  beginSingleTimeCommands(CommandCtx* commandCtx,VkDeviceCtx* deviceCtx);

  void endSingleTimeCommands(VkCommandBuffer* commandBuffer,CommandCtx* commandCtx,VkDeviceCtx* deviceCtx);

#ifdef __cplusplus
}
  
#endif

