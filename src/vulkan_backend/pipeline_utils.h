#pragma once

#include "vk_types.h"
#include "vk_pipeline.h"

VkShaderModule createShaderModule(VkDeviceCtx* deviceCtx,const char* code,int size);
void createTrianglePipeline(pipelineStateDescriptions* desc,VkPipelineCtx* pipelineCtx,VkDeviceCtx* deviceCtx);
void createLinePipeline(pipelineStateDescriptions* desc,VkPipelineCtx* pipelineCtx,VkDeviceCtx* deviceCtx);
