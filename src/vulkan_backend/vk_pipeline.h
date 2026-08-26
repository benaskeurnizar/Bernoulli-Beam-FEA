#pragma once

#include "vk_types.h"
#include "vk_swapChain.h"
#include "vk_device.h"
#include "vk_descriptor.h"
#include "../core/file.h"
#include "../graphics/graphics.h"

typedef struct{
  VkPipeline trianglePipeline;
  VkPipeline linePipeline;
  VkPipeline pointPipeline;
  VkPipelineLayout pipelineLayout;
  VkRenderPass renderPass;
  VkFramebuffer* frameBuffers;      
  uint32_t frameBufferCount;       
}VkPipelineCtx;

typedef struct{
  VkViewport viewport;
  VkRect2D scissor;
  
  VkPipelineViewportStateCreateInfo viewportState;
  VkPipelineDynamicStateCreateInfo dynamicState;
  VkPipelineRasterizationStateCreateInfo rasterizer;
  VkPipelineMultisampleStateCreateInfo multisampling;
  VkPipelineColorBlendAttachmentState colorBlendAttachment;
  VkPipelineColorBlendStateCreateInfo colorBlending;
  VkPipelineDepthStencilStateCreateInfo depthStencil;
}pipelineStateDescriptions;


void createPipelineCtx(VkPipelineCtx* pipelineCtx,VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx,DescriptorCtx* descriptorCtx);
void cleanupPipelineCtx(VkPipelineCtx* pipelineCtx,VkDeviceCtx* deviceCtx);
