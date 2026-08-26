#include "vk_sync.h"

void createSyncObjects(VkSyncCtx* syncCtx,VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx){
  syncCtx->render_finished = (VkSemaphore*)malloc(sizeof(VkSemaphore) * swapChainCtx->imageCount);
  syncCtx->image_available = (VkSemaphore*)malloc(sizeof(VkSemaphore) * MAX_FRAMES_IN_FLIGHT);
  syncCtx->in_flight = (VkFence*)malloc(sizeof(VkFence) * MAX_FRAMES_IN_FLIGHT);

  syncCtx->finishedCount = swapChainCtx->imageCount;
  syncCtx->availableCount = MAX_FRAMES_IN_FLIGHT;
   syncCtx->in_flightCount = MAX_FRAMES_IN_FLIGHT;
  
  VkSemaphoreCreateInfo semaphoreInfo = {0};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo = {0};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  bool ok = true;
  for(int i=0;i<MAX_FRAMES_IN_FLIGHT && ok;i++){
    ok = vkCreateSemaphore(deviceCtx->device, &semaphoreInfo, NULL, &(syncCtx->image_available[i]) ) == VK_SUCCESS && vkCreateFence(deviceCtx->device, &fenceInfo,NULL, &(syncCtx->in_flight[i])) == VK_SUCCESS;
  }
  for(int i=0;i<swapChainCtx->imageCount && ok;i++){
    ok = vkCreateSemaphore(deviceCtx->device, &semaphoreInfo, NULL, &(syncCtx->render_finished[i])) == VK_SUCCESS;
  }
  if(!ok){
    fprintf(stderr,"failed to create sync objects!\n");
    exit(EXIT_FAILURE);
  }
  
}


void cleanupSyncObjects(VkSyncCtx* syncCtx,VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx){
  for (uint32_t i = 0; i < swapChainCtx->imageCount; i++) {
    vkDestroySemaphore(deviceCtx->device, syncCtx->render_finished[i], NULL);
  }
  
  for(int i=0;i<MAX_FRAMES_IN_FLIGHT;i++){
    vkDestroySemaphore(deviceCtx->device, syncCtx->image_available[i], NULL);
    vkDestroyFence(deviceCtx->device, syncCtx->in_flight[i], NULL);
  }
}
