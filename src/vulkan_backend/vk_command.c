#include "vk_command.h"


void createCommandPool(CommandCtx* commandCtx,VkDeviceCtx* deviceCtx,VkSurfaceKHR surface){
  QueueFamilyIndices queueFamilyIndices = findQueueFamilies(surface,deviceCtx->physicalDevice);
  VkCommandPoolCreateInfo poolInfo = {0};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
  if(vkCreateCommandPool(deviceCtx->device,&poolInfo,NULL,&(commandCtx->pool)) != VK_SUCCESS){
    fprintf(stderr,"failed to create command pool\n");
    exit(EXIT_FAILURE);
  }
}

void allocateCommandBuffers(CommandCtx* commandCtx,VkDeviceCtx* deviceCtx){
  commandCtx->buffers = (VkCommandBuffer*)malloc(sizeof(VkCommandBuffer)*MAX_FRAMES_IN_FLIGHT);
  
  VkCommandBufferAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandCtx->pool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = (uint32_t)MAX_FRAMES_IN_FLIGHT;
  if(vkAllocateCommandBuffers(deviceCtx->device,&allocInfo,(commandCtx->buffers)) != VK_SUCCESS){
    fprintf(stderr,"failed to create command buffer\n");
    exit(EXIT_FAILURE);
  }
  
}


void createCommandCtx(CommandCtx* commandCtx,VkDeviceCtx* deviceCtx,VkSurfaceKHR surface){
  createCommandPool(commandCtx,deviceCtx,surface);
  allocateCommandBuffers(commandCtx,deviceCtx);
}

void cleanupCommandCtx(CommandCtx* commandCtx,VkDeviceCtx* deviceCtx){
  vkDestroyCommandPool(deviceCtx->device,commandCtx->pool,NULL);
}
