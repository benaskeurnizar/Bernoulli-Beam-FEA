#pragma once

#include "vk_types.h"
#include "vk_device.h"
#include "vk_buffer.h"

typedef struct {
  VkDescriptorSetLayout layout;
  VkDescriptorPool pool;
  VkDescriptorPool imguiPool;
  VkDescriptorSet* sets;   // array, size MAX_FRAMES_IN_FLIGHT
} DescriptorCtx;


void createDescriptorLayout(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx);
void createDescriptorPoolsSets(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx, FrameUniforms* meshUniforms,uint32_t meshCount,FrameUniforms* lineUniforms,uint32_t gridCount, FrameUniforms* globalUniform);

void cleanDescriptorLayout(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx);
void cleanupDescriptorPoolSets(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx);
