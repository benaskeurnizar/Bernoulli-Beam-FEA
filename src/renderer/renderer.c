#include "renderer.h"
#include "../platform/window.h"
#include "../vulkan_backend/vk_instance.h"
#include "../vulkan_backend/vk_device.h"
#include "../vulkan_backend/vk_descriptor.h"
#include "../vulkan_backend/vk_command.h"
#include "../vulkan_backend/vk_buffer.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "imgui_bridge.h"



void createSurface(Renderer* renderer,Window* window){
  if(glfwCreateWindowSurface(renderer->instanceCtx.instance,(window->window),NULL,&(renderer->surface))!=VK_SUCCESS){
    fprintf(stderr,"failed to create window surface! \n");
    exit(EXIT_FAILURE);
  }
}




void initRenderer(Renderer* renderer,Window* window,World* world){
  renderer->currentFrame = 0;
  renderer->framebufferResized = 0;
  createInstanceCtx(&(renderer->instanceCtx));
  createSurface(renderer,window);
  createDeviceCtx(&(renderer->instanceCtx),&(renderer->deviceCtx),renderer->surface);
  createSwapChainCtx(&(renderer->swapChainCtx),&(renderer->deviceCtx),window,renderer->surface);
  createDescriptorLayout(&(renderer->descriptorsCtx),&(renderer->deviceCtx));
  createPipelineCtx(&(renderer->pipelineCtx),&(renderer->swapChainCtx),&(renderer->deviceCtx),&(renderer->descriptorsCtx));
  createCommandCtx(&(renderer->commandsCtx),&(renderer->deviceCtx),renderer->surface);

  renderer->meshBuffersCount = world->meshCount;
  renderer->meshBuffers = (MeshBuffers*)calloc((int)renderer->meshBuffersCount,sizeof(MeshBuffers));
  //NOTE: the per mesh uniforms
  renderer->meshUniforms = (FrameUniforms*)calloc((int)renderer->meshBuffersCount,sizeof(FrameUniforms));
  VkDeviceSize meshUniformBufferSize = sizeof(UniformBufferMesh);
  for(uint32_t i=0;i<renderer->meshBuffersCount;i++){
    createMeshBuffers(&(renderer->meshBuffers[i]),&(renderer->deviceCtx),&(renderer->commandsCtx),&(world->meshes[i]));
    //NOTE: the per mesh uniforms
    createUniformBuffers(&(renderer->meshUniforms[i]),&(renderer->deviceCtx),&(renderer->commandsCtx),meshUniformBufferSize);
  }

  renderer->lineBuffersCount = world->gridCount;
  renderer->lineBuffers = (LineBuffer*)calloc((int)renderer->lineBuffersCount,sizeof(LineBuffer));
  renderer->lineUniforms = (FrameUniforms*)calloc((int)renderer->lineBuffersCount,sizeof(FrameUniforms));
  for(uint32_t i=0;i<renderer->lineBuffersCount;i++){
    createLineBuffer(&(renderer->lineBuffers[i]),&(renderer->deviceCtx),&(renderer->commandsCtx),&(world->grids[i]));
    createUniformBuffers(&(renderer->lineUniforms[i]),&(renderer->deviceCtx),&(renderer->commandsCtx),meshUniformBufferSize);
  }
  //NOTE: the global uniform
  VkDeviceSize globalUniformBufferSize = sizeof(UniformBufferGlobal);
  createUniformBuffers(&(renderer->globalUniform),&(renderer->deviceCtx),&(renderer->commandsCtx),globalUniformBufferSize);

  createDescriptorPoolsSets(&(renderer->descriptorsCtx),&(renderer->deviceCtx),(renderer->meshUniforms),world->meshCount,(renderer->lineUniforms),world->gridCount,&(renderer->globalUniform));

  createSyncObjects(&(renderer->syncCtx),&(renderer->swapChainCtx),&(renderer->deviceCtx));

  //NOTE: CREATING IMGUI
  imgui_init(renderer, window);
  //imgui_upload_fonts(renderer);
}


void cleanupRenderer(Renderer* renderer,World* world){
  vkDeviceWaitIdle(renderer->deviceCtx.device);

  imgui_destroy();

  cleanupSyncObjects(&(renderer->syncCtx),&(renderer->swapChainCtx),&(renderer->deviceCtx));
  cleanupDescriptorPoolSets(&(renderer->descriptorsCtx),&(renderer->deviceCtx));
  cleanUniformBuffer(&(renderer->globalUniform),&(renderer->deviceCtx));

  for(int i=0;i<world->gridCount;i++){
    vkDestroyBuffer((renderer->deviceCtx.device), renderer->lineBuffers[i].vertexBuffer, NULL);
    vkFreeMemory(renderer->deviceCtx.device, renderer->lineBuffers[i].vertexBufferMemory, NULL);
    for (uint32_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
      vkUnmapMemory(renderer->deviceCtx.device, renderer->lineUniforms[i].uniformBuffersMemory[j]);

      vkDestroyBuffer(renderer->deviceCtx.device, renderer->lineUniforms[i].uniformBuffers[j], NULL);
      vkFreeMemory(renderer->deviceCtx.device, renderer->lineUniforms[i].uniformBuffersMemory[j], NULL);
    }
  }
  
  
  for (uint32_t i = 0; i < world->meshCount; i++) {
    cleanUniformBuffer(&(renderer->meshUniforms[i]),&(renderer->deviceCtx));
    cleanMeshBuffers(&(renderer->meshBuffers[i]),&(renderer->deviceCtx),&(renderer->commandsCtx));
  }

  //destroy diagram buffers
  if(renderer->show_diagram){
    vkDestroyBuffer((renderer->deviceCtx.device), renderer->diagramBuffer.vertexBuffer, NULL);
    vkFreeMemory(renderer->deviceCtx.device, renderer->diagramBuffer.vertexBufferMemory, NULL);
    for (uint32_t j = 0; j < MAX_FRAMES_IN_FLIGHT; j++) {
      vkUnmapMemory(renderer->deviceCtx.device, renderer->diagramUniform.uniformBuffersMemory[j]);

      vkDestroyBuffer(renderer->deviceCtx.device, renderer->diagramUniform.uniformBuffers[j], NULL);
      vkFreeMemory(renderer->deviceCtx.device, renderer->diagramUniform.uniformBuffersMemory[j], NULL);
    }
  }
  free(renderer->meshUniforms);
  cleanupCommandCtx(&(renderer->commandsCtx),&(renderer->deviceCtx));
  cleanupPipelineCtx(&(renderer->pipelineCtx),&(renderer->deviceCtx));
  cleanDescriptorLayout(&(renderer->descriptorsCtx),&(renderer->deviceCtx));
  
  cleanupSwapChainCtx(&(renderer->swapChainCtx),&(renderer->deviceCtx));
  
  cleanupDeviceCtx(&(renderer->deviceCtx));
  vkDestroySurfaceKHR(renderer->instanceCtx.instance,renderer->surface,NULL);
  cleanupInstanceCtx(&(renderer->instanceCtx));
}

/*


1. Descriptor Set Layout
        ↓
2. Pipeline Layout
        ↓
3. Graphics Pipeline
        ↓
4. Descriptor Pool
        ↓
5. Descriptor Sets
        ↓
6. Update Descriptor Sets
        ↓
7. Render

 */
