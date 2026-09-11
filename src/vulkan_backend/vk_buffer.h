#pragma once

#include "../graphics/graphics.h"
#include "vk_types.h"
#include "vk_device.h"
#include "vk_command.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct {
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;
} MeshBuffers;   // one-time, static geometry data

typedef struct {
  VkBuffer* uniformBuffers;           // array, size MAX_FRAMES_IN_FLIGHT
  VkDeviceMemory* uniformBuffersMemory;
  void** uniformBuffersMapped;
} FrameUniforms;   // per-frame-in-flight data

typedef struct {
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
} LineBuffer;


void createMeshBuffers(MeshBuffers* meshBuffers,VkDeviceCtx* device,CommandCtx* commandCtx,Mesh* mesh);
void createUniformBuffers(FrameUniforms* frameUniform,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx,VkDeviceSize bufferSize);
void createLineBuffer(LineBuffer* lineBuffer,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx,Grid* grid);
void createDiagramBuffer(LineBuffer* lineBuffer,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx,Diagram* diagram);

#ifdef __cplusplus
}
#endif
