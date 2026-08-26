#include "vk_buffer.h"


uint32_t findMemoryType(VkDeviceCtx* deviceCtx,uint32_t typeFilter, VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(deviceCtx->physicalDevice, &memProperties);
  for(uint32_t i=0;i<memProperties.memoryTypeCount;i++){
    if(typeFilter & (1<<i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties ){
      return i;
    }
  }
  fprintf(stderr,"failed to find suitable memory type!\n");
  exit(EXIT_FAILURE);
}

void createBuffer(VkDeviceCtx* deviceCtx,VkDeviceSize size,VkBufferUsageFlags usage,VkMemoryPropertyFlags properties,VkBuffer* buffer,VkDeviceMemory* bufferMemory){
  VkBufferCreateInfo bufferInfo = {0};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size; //sizeof(vertices[0])*vertexCount;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if(vkCreateBuffer(deviceCtx->device,&bufferInfo,NULL,(buffer))!=VK_SUCCESS){
    fprintf(stderr,"failed to create vertex buffer!\n");
    exit(EXIT_FAILURE);
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(deviceCtx->device, *buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(deviceCtx,memRequirements.memoryTypeBits,properties);

  if(vkAllocateMemory(deviceCtx->device,&allocInfo,NULL,(bufferMemory))!=VK_SUCCESS){
    fprintf(stderr,"failed to allocate vertex buffer memory!\n");
    exit(EXIT_FAILURE);
  }

  vkBindBufferMemory(deviceCtx->device,*buffer,*bufferMemory,0);
}

void copyBuffer(CommandCtx* commandCtx,VkDeviceCtx* deviceCtx,VkBuffer srcBuffer,VkBuffer dstBuffer,VkDeviceSize size){
  // we must first allocatea temporary commandbuffe
  VkCommandBufferAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = commandCtx->pool;
  allocInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer;
  vkAllocateCommandBuffers(deviceCtx->device,&allocInfo,&commandBuffer);

  // and immidiatly start recording the command buffer
  VkCommandBufferBeginInfo beginInfo = {0};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(commandBuffer,&beginInfo);

  VkBufferCopy copyRegion = {0};
  copyRegion.srcOffset = 0;//Optional
  copyRegion.dstOffset = 0;//Optional
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer,srcBuffer,dstBuffer,1,&copyRegion);
  vkEndCommandBuffer(commandBuffer);

  VkSubmitInfo submitInfo = {0};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;
  vkQueueSubmit(deviceCtx->graphicsQueue,1,&submitInfo,VK_NULL_HANDLE);
  vkQueueWaitIdle(deviceCtx->graphicsQueue);

  vkFreeCommandBuffers(deviceCtx->device,commandCtx->pool,1,&commandBuffer);
}

void createVertexBuffer(MeshBuffers* meshBuffers,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx,Mesh* mesh){
  VkDeviceSize bufferSize = sizeof(mesh->vertices[0])*mesh->vertexCount;
  VkBuffer stagingBuffer = {0};
  VkDeviceMemory stagingBufferMemory = {0};
  createBuffer(deviceCtx,bufferSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,&stagingBuffer,&stagingBufferMemory);

  //NOTE: Filling the vertex buffer
  // mapping the buffer memory into CPU accessible memory with vkMapMemory.
  void* data;
  vkMapMemory(deviceCtx->device, stagingBufferMemory, 0, bufferSize, 0, &data);
  // This function allows us to access a region of the specified memory resource defined by an offset and size. 
  // Theoffset and size here are 0 and bufferInfo.size
  memcpy(data,mesh->vertices,(size_t)bufferSize);
  vkUnmapMemory(deviceCtx->device, stagingBufferMemory);

  createBuffer(deviceCtx,bufferSize,VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,&(meshBuffers->vertexBuffer),&(meshBuffers->vertexBufferMemory));
  // we can now call copy Buffer from the create Vertex Buffer function to move the vertex data to the device local buffer
  copyBuffer(commandCtx,deviceCtx,stagingBuffer,meshBuffers->vertexBuffer,bufferSize);
  vkDestroyBuffer(deviceCtx->device,stagingBuffer,NULL);
  vkFreeMemory(deviceCtx->device,stagingBufferMemory,NULL);
}

void createIndexBuffer(MeshBuffers* meshBuffers,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx,Mesh* mesh){
  VkDeviceSize bufferSize = sizeof(mesh->indices[0])*mesh->indexCount;
  VkBuffer stagingBuffer = {0};
  VkDeviceMemory stagingBufferMemory = {0};
  createBuffer(deviceCtx,bufferSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,&stagingBuffer,&stagingBufferMemory);

  //NOTE: Filling the vertex buffer
  // mapping the buffer memory into CPU accessible memory with vkMapMemory.
  void* data;
  vkMapMemory(deviceCtx->device, stagingBufferMemory, 0, bufferSize, 0, &data);
  // This function allows us to access a region of the specified memory resource defined by an offset and size. 
  // Theoffset and size here are 0 and bufferInfo.size
  memcpy(data,mesh->indices,(size_t)bufferSize);
  vkUnmapMemory(deviceCtx->device, stagingBufferMemory);

  createBuffer(deviceCtx,bufferSize,VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,&(meshBuffers->indexBuffer),&(meshBuffers->indexBufferMemory));
  // we can now call copy Buffer from the create Vertex Buffer function to move the vertex data to the device local buffer
  copyBuffer(commandCtx,deviceCtx,stagingBuffer,meshBuffers->indexBuffer,bufferSize);
  vkDestroyBuffer(deviceCtx->device,stagingBuffer,NULL);
  vkFreeMemory(deviceCtx->device,stagingBufferMemory,NULL);
}

void createUniformBuffers(FrameUniforms* frameUniform,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx,VkDeviceSize bufferSize){
   frameUniform->uniformBuffers = (VkBuffer*)malloc(sizeof(VkBuffer)*MAX_FRAMES_IN_FLIGHT);
   frameUniform->uniformBuffersMemory = (VkDeviceMemory*)malloc(sizeof(VkDeviceMemory)*MAX_FRAMES_IN_FLIGHT);
   frameUniform->uniformBuffersMapped = (void**)malloc(sizeof(void*)*MAX_FRAMES_IN_FLIGHT);

   for(size_t i = 0;i < MAX_FRAMES_IN_FLIGHT;i++){
     createBuffer(deviceCtx,bufferSize,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,&(frameUniform->uniformBuffers[i]),&(frameUniform->uniformBuffersMemory[i]));

     vkMapMemory(deviceCtx->device,frameUniform->uniformBuffersMemory[i],0,bufferSize,0,&(frameUniform->uniformBuffersMapped[i]));
   }
}


void createMeshBuffers(MeshBuffers* meshBuffers,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx,Mesh* mesh){
  createVertexBuffer(meshBuffers,deviceCtx,commandCtx,mesh);
  createIndexBuffer(meshBuffers,deviceCtx,commandCtx,mesh);
}

void createLineBuffer(LineBuffer* lineBuffer,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx,Grid* grid){
  VkDeviceSize bufferSize = sizeof(grid->vertices[0])*grid->vertexCount;
  VkBuffer stagingBuffer = {0};
  VkDeviceMemory stagingBufferMemory = {0};
  createBuffer(deviceCtx,bufferSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,&stagingBuffer,&stagingBufferMemory);

  //NOTE: Filling the vertex buffer
  // mapping the buffer memory into CPU accessible memory with vkMapMemory.
  void* data;
  vkMapMemory(deviceCtx->device, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data,grid->vertices,(size_t)bufferSize);
  vkUnmapMemory(deviceCtx->device, stagingBufferMemory);

  createBuffer(deviceCtx,bufferSize,VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,&(lineBuffer->vertexBuffer),&(lineBuffer->vertexBufferMemory));
  // we can now call copy Buffer from the create Vertex Buffer function to move the vertex data to the device local buffer
  copyBuffer(commandCtx,deviceCtx,stagingBuffer,lineBuffer->vertexBuffer,bufferSize);
  vkDestroyBuffer(deviceCtx->device,stagingBuffer,NULL);
  vkFreeMemory(deviceCtx->device,stagingBufferMemory,NULL);
}

void cleanMeshBuffers(MeshBuffers* meshBuffers,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx){
  vkDestroyBuffer(deviceCtx->device, meshBuffers->vertexBuffer, NULL);
  vkFreeMemory(deviceCtx->device, meshBuffers->vertexBufferMemory, NULL);

  vkDestroyBuffer(deviceCtx->device, meshBuffers->indexBuffer, NULL);
  vkFreeMemory(deviceCtx->device, meshBuffers->indexBufferMemory, NULL);
}

void cleanUniformBuffer(FrameUniforms* frameUniform,VkDeviceCtx* deviceCtx){
  for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkUnmapMemory(deviceCtx->device, frameUniform->uniformBuffersMemory[i]);

    vkDestroyBuffer(deviceCtx->device, frameUniform->uniformBuffers[i], NULL);
    vkFreeMemory(deviceCtx->device, frameUniform->uniformBuffersMemory[i], NULL);
  }

  
  
  free(frameUniform->uniformBuffers);
  free(frameUniform->uniformBuffersMemory);
  free(frameUniform->uniformBuffersMapped);
}
