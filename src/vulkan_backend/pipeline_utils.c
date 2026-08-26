#include "vk_types.h"
#include "vk_pipeline.h"

VkShaderModule createShaderModule(VkDeviceCtx* deviceCtx,const char* code,int size){
  if((uintptr_t)code % 4 !=0){
    fprintf(stderr,"unaligned memory passed to createShaderModule\n");
    exit(EXIT_FAILURE);
  }
  VkShaderModuleCreateInfo createInfo = {0};
  createInfo.sType= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = size;
  createInfo.pCode = (const uint32_t*)code;
  
  VkShaderModule shaderModule;
  if(vkCreateShaderModule(deviceCtx->device,&createInfo,NULL,&shaderModule)!=VK_SUCCESS){
    fprintf(stderr, "failed to create shader module ! \n");
    exit(EXIT_FAILURE);
  }
  return shaderModule;
}

void createTrianglePipeline(pipelineStateDescriptions* desc,VkPipelineCtx* pipelineCtx,VkDeviceCtx* deviceCtx){
  int vert_size = 0;
  char* vertShaderCode = readShaderFile(&vert_size,"../shaders/vert.spv");
  int frag_size = 0;
  char* fragShaderCode = readShaderFile(&frag_size,"../shaders/frag.spv");
  
  VkShaderModule vertShaderModule = createShaderModule(deviceCtx,vertShaderCode,vert_size);
  VkShaderModule fragShaderModule = createShaderModule(deviceCtx,fragShaderCode,frag_size);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo = {0};
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo = {0};
  fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

  //NOTE: Descritions and attribute descriptions
  VkVertexInputBindingDescription bindingDescription = getTriangleBindingDescription();
  VkVertexInputAttributeDescription attributeDescriptions[3];
  getTriangleAttributeDescriptions(attributeDescriptions);

  //Vertex input
  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {0};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; // Optional
  vertexInputInfo.vertexAttributeDescriptionCount = 3;
  vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions; // Optional

  //Input Assemly
  VkPipelineInputAssemblyStateCreateInfo inputAssembly = {0};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  // The graphics pipeline
  VkGraphicsPipelineCreateInfo pipelineInfo = {0};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &desc->viewportState;
  pipelineInfo.pRasterizationState = &desc->rasterizer;
  pipelineInfo.pMultisampleState = &desc->multisampling;
  pipelineInfo.pDepthStencilState = &desc->depthStencil; // Optional
  pipelineInfo.pColorBlendState = &desc->colorBlending;
  pipelineInfo.pDynamicState = &desc->dynamicState;
  pipelineInfo.layout = pipelineCtx->pipelineLayout;
  pipelineInfo.renderPass = pipelineCtx->renderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
  pipelineInfo.basePipelineIndex =-1; // Optional
  if(vkCreateGraphicsPipelines(deviceCtx->device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL,&(pipelineCtx->trianglePipeline)) != VK_SUCCESS){
    fprintf(stderr,"failed to create graphics pipeline!\n");
    exit(EXIT_FAILURE);
  }
  
  free(vertShaderCode);
  free(fragShaderCode);
  vkDestroyShaderModule(deviceCtx->device,fragShaderModule,NULL);
  vkDestroyShaderModule(deviceCtx->device,vertShaderModule,NULL);
}


void createLinePipeline(pipelineStateDescriptions* desc,VkPipelineCtx* pipelineCtx,VkDeviceCtx* deviceCtx){
  int vert_size = 0;
  char* vertShaderCode = readShaderFile(&vert_size,"../shaders/lineVert.spv");
  int frag_size = 0;
  char* fragShaderCode = readShaderFile(&frag_size,"../shaders/lineFrag.spv");
  
  VkShaderModule vertShaderModule = createShaderModule(deviceCtx,vertShaderCode,vert_size);
  VkShaderModule fragShaderModule = createShaderModule(deviceCtx,fragShaderCode,frag_size);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo = {0};
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo = {0};
  fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

  //NOTE: Descritions and attribute descriptions
  VkVertexInputBindingDescription bindingDescription = getLineBindingDescription();
  VkVertexInputAttributeDescription attributeDescriptions;
  getLineAttributeDescriptions(&attributeDescriptions);

  //Vertex input
  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {0};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; // Optional
  vertexInputInfo.vertexAttributeDescriptionCount = 1;
  vertexInputInfo.pVertexAttributeDescriptions = &attributeDescriptions; // Optional

  //Input Assemly
  VkPipelineInputAssemblyStateCreateInfo inputAssembly = {0};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  // The graphics pipeline
  VkGraphicsPipelineCreateInfo pipelineInfo = {0};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &desc->viewportState;
  pipelineInfo.pRasterizationState = &desc->rasterizer;
  pipelineInfo.pMultisampleState = &desc->multisampling;
  pipelineInfo.pDepthStencilState = &desc->depthStencil; // Optional
  pipelineInfo.pColorBlendState = &desc->colorBlending;
  pipelineInfo.pDynamicState = &desc->dynamicState;
  pipelineInfo.layout = pipelineCtx->pipelineLayout;
  pipelineInfo.renderPass = pipelineCtx->renderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
  pipelineInfo.basePipelineIndex =-1; // Optional
  if(vkCreateGraphicsPipelines(deviceCtx->device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL,&(pipelineCtx->linePipeline)) != VK_SUCCESS){
    fprintf(stderr,"failed to create graphics pipeline!\n");
    exit(EXIT_FAILURE);
  }
  
  free(vertShaderCode);
  free(fragShaderCode);
  vkDestroyShaderModule(deviceCtx->device,fragShaderModule,NULL);
  vkDestroyShaderModule(deviceCtx->device,vertShaderModule,NULL);
}
