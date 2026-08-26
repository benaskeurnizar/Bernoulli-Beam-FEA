
#include "draw.h"


void recreateSwapChain(VkSwapchainCtx* swapChainCtx,VkPipelineCtx* pipelineCtx,VkDeviceCtx* deviceCtx,Window* window,VkSurfaceKHR surface){
  int width = 0,height = 0;
  glfwGetFramebufferSize(window->window,&width,&height);
  while(width==0 || height==0){
    glfwGetFramebufferSize(window->window,&width,&height);
    glfwWaitEvents();
  }
  vkDeviceWaitIdle(deviceCtx->device);

  cleanupSwapChainCtx(swapChainCtx,deviceCtx);
  for(uint32_t i=0;i<pipelineCtx->frameBufferCount;i++){
    vkDestroyFramebuffer(deviceCtx->device,pipelineCtx->frameBuffers[i],NULL);
  }

  createSwapChainCtx(swapChainCtx,deviceCtx,window,surface);
  createFramebuffers(pipelineCtx,swapChainCtx,deviceCtx);
}


void updateUniformBuffer(World* world, FrameUniforms* globalUniform, FrameUniforms* meshUniforms, FrameUniforms* lineUniform, uint32_t currentImage){
    UniformBufferGlobal ubog = {0};
    glm_mat4_copy(world->camera.view, ubog.view);
    glm_mat4_copy(world->proj, ubog.proj);
    glm_vec_copy(world->camera.pos,ubog.cam_pos);
    memcpy(globalUniform->uniformBuffersMapped[currentImage], &ubog, sizeof(ubog));

    for (uint32_t i = 0; i < world->meshCount; i++) {
        UniformBufferMesh ubom = {0};
        glm_mat4_copy(world->meshes[i].model, ubom.model);
        memcpy(meshUniforms[i].uniformBuffersMapped[currentImage], &ubom, sizeof(ubom));
    }

    UniformBufferMesh ubol = {0};
    glm_mat4_identity(ubol.model);
    memcpy(lineUniform->uniformBuffersMapped[currentImage], &ubol, sizeof(ubol));
}

void recordCommandBuffer(Renderer* renderer,
                         World* world,
                         uint32_t imageIndex)
{
  VkCommandBuffer commandBuffer =
    renderer->commandsCtx.buffers[renderer->currentFrame];

  VkCommandBufferBeginInfo beginInfo = {0};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    fprintf(stderr, "failed to begin recording command buffer!\n");
    exit(EXIT_FAILURE);
  }

  VkRenderPassBeginInfo renderPassInfo = {0};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = renderer->pipelineCtx.renderPass;
  renderPassInfo.framebuffer = renderer->pipelineCtx.frameBuffers[imageIndex];
  renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
  renderPassInfo.renderArea.extent = renderer->swapChainCtx.swapChainExtent;

  
  VkClearValue clearValues[3] = {0};
  clearValues[0].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
  clearValues[1].depthStencil.depth = 1.0f;
  clearValues[1].depthStencil.stencil = 0;
  clearValues[2].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
  
  renderPassInfo.clearValueCount = 3;
  renderPassInfo.pClearValues = clearValues;

  vkCmdBeginRenderPass(commandBuffer,
		       &renderPassInfo,
		       VK_SUBPASS_CONTENTS_INLINE);

  
  vkCmdBindPipeline(commandBuffer,
		    VK_PIPELINE_BIND_POINT_GRAPHICS,
		    renderer->pipelineCtx.linePipeline);
  
  VkViewport viewport = {0};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)renderer->swapChainCtx.swapChainExtent.width;
  viewport.height = (float)renderer->swapChainCtx.swapChainExtent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor = {0};
  scissor.offset = (VkOffset2D){0, 0};
  scissor.extent = renderer->swapChainCtx.swapChainExtent;
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  //TODO: DRAW THE LINE GRIDS HERE
  VkDeviceSize offsets[] = {0};
  for(int i=0;i<world->gridCount;i++){
    vkCmdBindVertexBuffers(commandBuffer,
			   0,
			   1,
			 &renderer->lineBuffers[i].vertexBuffer,
			 offsets);
    uint32_t lineSet =
      renderer->meshBuffersCount * MAX_FRAMES_IN_FLIGHT +
      renderer->currentFrame;
  
    vkCmdBindDescriptorSets(
			    commandBuffer,
			    VK_PIPELINE_BIND_POINT_GRAPHICS,
			    renderer->pipelineCtx.pipelineLayout,
			    0,
			    1,
			    &renderer->descriptorsCtx.sets[lineSet],
			    0,
			    NULL);
    vkCmdDraw(
	      commandBuffer,
	      world->grids[i].vertexCount,
	      1,
	      0,
	      0);
  }
  //NOTE: draw the triangles
  
  vkCmdBindPipeline(commandBuffer,
		    VK_PIPELINE_BIND_POINT_GRAPHICS,
		    renderer->pipelineCtx.trianglePipeline);

  for (uint32_t i = 0; i < world->meshCount; i++) {

    VkBuffer vertexBuffers[] = {
      renderer->meshBuffers[i].vertexBuffer
    };
    

    vkCmdBindVertexBuffers(commandBuffer,
			   0,
			   1,
			   vertexBuffers,
			   offsets);

    vkCmdBindIndexBuffer(commandBuffer,
			 renderer->meshBuffers[i].indexBuffer,
			 0,
			 VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(
			    commandBuffer,
			    VK_PIPELINE_BIND_POINT_GRAPHICS,
			    renderer->pipelineCtx.pipelineLayout,
			    0,
			    1,
			    &renderer->descriptorsCtx.sets[
							   i * MAX_FRAMES_IN_FLIGHT + renderer->currentFrame
							   ],
			    0,
			    NULL);

    vkCmdDrawIndexed(commandBuffer,
		     world->meshes[i].indexCount,
		     1,
		     0,
		     0,
		     0);
  }

  vkCmdEndRenderPass(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    fprintf(stderr, "failed to record command buffer!\n");
    exit(EXIT_FAILURE);
  }
}

void drawFrame(Renderer* renderer,Window* window,World* world){
  vkWaitForFences(renderer->deviceCtx.device, 1, &(renderer->syncCtx.in_flight[renderer->currentFrame]), VK_TRUE, UINT64_MAX);

  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(renderer->deviceCtx.device,renderer->swapChainCtx.swapChain,UINT64_MAX,renderer->syncCtx.image_available[renderer->currentFrame],VK_NULL_HANDLE,&imageIndex);
  if(result == VK_ERROR_OUT_OF_DATE_KHR ){
    recreateSwapChain(&(renderer->swapChainCtx),&(renderer->pipelineCtx),&(renderer->deviceCtx),window,renderer->surface);
    return;
  }else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
    fprintf(stderr,"failed to acquire swap chain image!\n");
    exit(EXIT_FAILURE);
  }
  if(world->update_beam_buffers){
    world->update_beam_buffers = 0;
    //NOTE: UPDATE THE MESH BUFFER
    vkDeviceWaitIdle(renderer->deviceCtx.device);
    //TODO: FOR NOW...WE KNOW IT'S AT INDEX 0
    for(int i=0;i<world->meshCount;i++){
      vkDestroyBuffer(renderer->deviceCtx.device, renderer->meshBuffers[i].vertexBuffer, NULL);
      vkFreeMemory(renderer->deviceCtx.device, renderer->meshBuffers[i].vertexBufferMemory, NULL);
      vkDestroyBuffer(renderer->deviceCtx.device, renderer->meshBuffers[i].indexBuffer, NULL);
      vkFreeMemory(renderer->deviceCtx.device, renderer->meshBuffers[i].indexBufferMemory, NULL);
      createMeshBuffers( &(renderer->meshBuffers[i]),&(renderer->deviceCtx),&(renderer->commandsCtx),&(world->meshes[i]));
    }
  }
  if(world->new_grid){
    world->new_grid = 0;
    int new_index = world->gridCount-1;
    VkDeviceSize meshUniformBufferSize = sizeof(UniformBufferMesh);
    //TODO: DELETE THE DESCRIPTOR SETS AND RECREATE THEM
    vkDeviceWaitIdle(renderer->deviceCtx.device);
    renderer->lineBuffersCount = world->gridCount;
    renderer->lineBuffers = (LineBuffer*)realloc(renderer->lineBuffers,sizeof(LineBuffer)*(int)renderer->lineBuffersCount);
    renderer->lineUniforms = (FrameUniforms*)realloc(renderer->lineUniforms,(int)renderer->lineBuffersCount*sizeof(FrameUniforms));
    createLineBuffer(&(renderer->lineBuffers[new_index]),&(renderer->deviceCtx),&(renderer->commandsCtx),&(world->grids[new_index]));
    createUniformBuffers(&(renderer->lineUniforms[new_index]),&(renderer->deviceCtx),&(renderer->commandsCtx),meshUniformBufferSize);
    
  }
  //Only reset the fence if we are submitting work
  vkResetFences(renderer->deviceCtx.device, 1, &(renderer->syncCtx.in_flight[renderer->currentFrame]));
  //updateUniformBuffer(world,&(renderer->globalUniform),renderer->meshUniforms,renderer->currentFrame);

  updateUniformBuffer(world, &(renderer->globalUniform), renderer->meshUniforms, (renderer->lineUniforms), renderer->currentFrame);
  vkResetCommandBuffer(renderer->commandsCtx.buffers[renderer->currentFrame],0);
  recordCommandBuffer(renderer,world,imageIndex);
    
  
  //submitting the command buffer
  VkSubmitInfo submitInfo = {0};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkSemaphore waitSemaphores[] = {renderer->syncCtx.image_available[renderer->currentFrame]};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &(renderer->commandsCtx.buffers[renderer->currentFrame]);
  submitInfo.signalSemaphoreCount = 1;
  VkSemaphore signalSemaphores[] = {renderer->syncCtx.render_finished[imageIndex]};
  submitInfo.pSignalSemaphores = signalSemaphores;
  if(vkQueueSubmit(renderer->deviceCtx.graphicsQueue, 1, &submitInfo, renderer->syncCtx.in_flight[renderer->currentFrame]) != VK_SUCCESS){
    fprintf(stderr,"failed to submit draw command buffer!\n");
    exit(EXIT_FAILURE);
  }

  VkPresentInfoKHR presentInfo = {0};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;
  VkSwapchainKHR swapChains[] = {renderer->swapChainCtx.swapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = NULL; // Optional
  result = vkQueuePresentKHR(renderer->deviceCtx.presentQueue, &presentInfo);
  if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || renderer->framebufferResized || world->resized){
    world->resized = 0;
    renderer->framebufferResized = false;
    recreateSwapChain(&(renderer->swapChainCtx),&(renderer->pipelineCtx),&(renderer->deviceCtx),window,renderer->surface);
  }else if(result != VK_SUCCESS){
    fprintf(stderr,"failed to present swap chain image!\n");
    exit(EXIT_FAILURE);
  }
  
  renderer->currentFrame = (renderer->currentFrame+1)%MAX_FRAMES_IN_FLIGHT;
 
}


/*

  
void recordCommandBuffer(VkSwapchainCtx* swapChainCtx,VkPipelineCtx* pipelineCtx,DescriptorCtx* descriptorCtx, MeshBuffers* meshBuffers,Mesh* meshes,uint32_t meshCount,VkCommandBuffer commandBuffer,uint32_t imageIndex,uint32_t currentFrame){
  VkCommandBufferBeginInfo beginInfo = {0};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;//Optional
  beginInfo.pInheritanceInfo = NULL;//Optional
  if(vkBeginCommandBuffer(commandBuffer,&beginInfo) != VK_SUCCESS){
    fprintf(stderr,"failed to begin recording command buffer!\n");
    exit(EXIT_FAILURE);
  }

  //Starting a render pass
  VkRenderPassBeginInfo renderPassInfo = {0};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = pipelineCtx->renderPass;
  renderPassInfo.framebuffer = pipelineCtx->frameBuffers[imageIndex];
  renderPassInfo.renderArea.offset.x = 0;
  renderPassInfo.renderArea.offset.y = 0;
  renderPassInfo.renderArea.extent = swapChainCtx->swapChainExtent;
  
  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;
  
  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
  //Basic drawing commands
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineCtx->pipeline);//bind the graphics pipeline
  // we did specify viewport and scissor state for this pipeline to be dynamic
  // So we need to set them in the command buffer before issuing our draw command
  VkViewport viewport = {0};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float) swapChainCtx->swapChainExtent.width;
  viewport.height = (float) swapChainCtx->swapChainExtent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor = {0};
  scissor.offset.x = 0;
  scissor.offset.y = 0;
  scissor.extent = swapChainCtx->swapChainExtent;
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  for(uint32_t i=0;i<meshCount;i++){
    VkBuffer vertexBuffers[] = {meshBuffers[i].vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, meshBuffers[i].indexBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineCtx->pipelineLayout, 0, 1,&descriptorCtx->sets[i * MAX_FRAMES_IN_FLIGHT +currentFrame], 0, NULL);
  
    //vkCmdDraw(commandBuffer, (uint32_t)vertexCount, 1, 0, 0); // vertexCount,instanceCount,firstVertex,firstInstance
    vkCmdDrawIndexed(commandBuffer,(uint32_t)meshes[i].indexCount, 1, 0, 0, 0);
  }
  
  vkCmdEndRenderPass(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    fprintf(stderr,"failed to record command buffer!\n");
    exit(EXIT_FAILURE);
  }
}



*/
