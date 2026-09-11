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

VkCommandBuffer  beginSingleTimeCommands(CommandCtx* commandCtx,VkDeviceCtx* deviceCtx){
  VkCommandBufferAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandCtx->pool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  if(vkAllocateCommandBuffers(deviceCtx->device,&allocInfo,&commandBuffer) != VK_SUCCESS){
    fprintf(stderr,"failed to begin single time command buffer\n");
    exit(EXIT_FAILURE);
  }

  VkCommandBufferBeginInfo beginInfo = {0};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    
  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  return commandBuffer;
  
}

void endSingleTimeCommands(VkCommandBuffer* commandBuffer,CommandCtx* commandCtx,VkDeviceCtx* deviceCtx){
  VkResult result =  vkEndCommandBuffer(*commandBuffer);
  if(result!=VK_SUCCESS){
    fprintf(stderr,"failed to END single time command buffer\n");
    exit(EXIT_FAILURE);
  }
  vkFreeCommandBuffers(
		       deviceCtx->device,
		       commandCtx->pool,
		       1,
		       commandBuffer
		       );
}

void cleanupCommandCtx(CommandCtx* commandCtx,VkDeviceCtx* deviceCtx){
  vkDestroyCommandPool(deviceCtx->device,commandCtx->pool,NULL);
}
