#include "vk_descriptor.h"
#include "vk_device.h"


void createDescriptorLayout(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx){
  VkDescriptorSetLayoutBinding uboLayoutBinding1 = {0};
  uboLayoutBinding1.binding = 0;
  uboLayoutBinding1.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding1.descriptorCount = 1;
  uboLayoutBinding1.stageFlags=VK_SHADER_STAGE_VERTEX_BIT|VK_SHADER_STAGE_FRAGMENT_BIT;
  uboLayoutBinding1.pImmutableSamplers = NULL;//Optional

  VkDescriptorSetLayoutBinding uboLayoutBinding2 = {0};
  uboLayoutBinding2.binding = 1;
  uboLayoutBinding2.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding2.descriptorCount = 1;
  uboLayoutBinding2.stageFlags=VK_SHADER_STAGE_VERTEX_BIT;
  uboLayoutBinding2.pImmutableSamplers = NULL;//Optional

  VkDescriptorSetLayoutBinding bindings[] = {uboLayoutBinding1,uboLayoutBinding2};
  VkDescriptorSetLayoutCreateInfo layoutInfo = {0};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = 2;
  layoutInfo.pBindings = bindings; 
  if(vkCreateDescriptorSetLayout(deviceCtx->device,&layoutInfo,NULL,&(descriptorsCtx->layout)) != VK_SUCCESS ){
     fprintf(stderr,"failed to create descriptor set layout !\n");
     exit(EXIT_FAILURE);
  }
}


void createDescriptorPool(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx,uint32_t meshCount){
  uint32_t descriptorCount = (meshCount+1+1) * MAX_FRAMES_IN_FLIGHT;
  VkDescriptorPoolSize poolSize = {0};
  poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSize.descriptorCount = descriptorCount*2;

  VkDescriptorPoolCreateInfo poolInfo = {0};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = 1;
  poolInfo.pPoolSizes = &poolSize;
  poolInfo.maxSets = descriptorCount;

  if(vkCreateDescriptorPool(deviceCtx->device,&poolInfo,NULL,&(descriptorsCtx->pool))!=VK_SUCCESS){
    fprintf(stderr,"failed to create descriptor pool!\n");
    exit(EXIT_FAILURE);
  }
}

void createImguiDescriptorPool(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx){
  VkDescriptorPoolSize pool_sizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
					{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
					{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
					{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
					{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
					{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
					{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
					{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
					{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
					{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
					{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };

  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = 1000;
  pool_info.poolSizeCount = (uint32_t)(sizeof(pool_sizes)/sizeof(VkDescriptorPoolSize));
  pool_info.pPoolSizes = pool_sizes;

  if(vkCreateDescriptorPool(deviceCtx->device, &pool_info,NULL, &(descriptorsCtx->imguiPool)) != VK_SUCCESS){
    fprintf(stderr,"failed to create the imgui descriptor pool!\n");
    exit(EXIT_FAILURE);
  }
}

void createDescriptorSets(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx, FrameUniforms* meshUniforms,FrameUniforms* lineUniforms,uint32_t meshCount,uint32_t gridCount, FrameUniforms* globalUniform){

  uint32_t descriptorCount = (meshCount+gridCount+1) * MAX_FRAMES_IN_FLIGHT;
  //VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT];
  VkDescriptorSetLayout* layouts = (VkDescriptorSetLayout*)malloc(sizeof(VkDescriptorSetLayout) *descriptorCount);

  for (int i = 0; i < (descriptorCount); i++) {
    layouts[i] = descriptorsCtx->layout;
  }

  VkDescriptorSetAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorsCtx->pool;
  allocInfo.descriptorSetCount = (uint32_t)descriptorCount;
  allocInfo.pSetLayouts = layouts;

 
  descriptorsCtx->sets = (VkDescriptorSet*)malloc(sizeof(VkDescriptorSet)*descriptorCount);
  if(vkAllocateDescriptorSets(deviceCtx->device,&allocInfo,descriptorsCtx->sets)!=VK_SUCCESS){
    fprintf(stderr,"failed to create descriptor sets!\n");
    exit(EXIT_FAILURE);
  }
  uint32_t setIndex = 0;
  //NOTE: NOW THE MESH UNIFORMS
  for(uint32_t i=0;i<meshCount;i++){
    for(size_t j=0;j<MAX_FRAMES_IN_FLIGHT;j++){
      VkDescriptorBufferInfo bufferInfoGlobal = {0};
      bufferInfoGlobal.buffer = globalUniform->uniformBuffers[j];
      bufferInfoGlobal.offset = 0;
      bufferInfoGlobal.range = sizeof(UniformBufferGlobal);

      VkWriteDescriptorSet descriptorWrite = {0};
      descriptorWrite.sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet= descriptorsCtx->sets[setIndex];
      descriptorWrite.dstBinding=0;
      descriptorWrite.dstArrayElement=0;
      descriptorWrite.descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrite.descriptorCount=1;
      descriptorWrite.pBufferInfo = &bufferInfoGlobal;
      descriptorWrite.pImageInfo = NULL; // Optional
      descriptorWrite.pTexelBufferView = NULL; // Optional
      vkUpdateDescriptorSets(deviceCtx->device, 1, &descriptorWrite, 0,NULL);

     
      VkDescriptorBufferInfo bufferInfoMesh = {0};
      bufferInfoMesh.buffer = meshUniforms[i].uniformBuffers[j];
      bufferInfoMesh.offset = 0;
      bufferInfoMesh.range = sizeof(UniformBufferMesh);

      descriptorWrite.sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet= descriptorsCtx->sets[setIndex];
      descriptorWrite.dstBinding=1;
      descriptorWrite.dstArrayElement=0;
      descriptorWrite.descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrite.descriptorCount=1;
      descriptorWrite.pBufferInfo = &bufferInfoMesh;
      descriptorWrite.pImageInfo = NULL; // Optional
      descriptorWrite.pTexelBufferView = NULL; // Optional
      vkUpdateDescriptorSets(deviceCtx->device, 1, &descriptorWrite, 0,NULL);

      setIndex++;
    }
  }

  //NOTE: FOR THE LINE
  for(int i=0;i<gridCount;i++){
    for(size_t j=0;j<MAX_FRAMES_IN_FLIGHT;j++){
      VkDescriptorBufferInfo bufferInfoGlobal = {0};
      bufferInfoGlobal.buffer = globalUniform->uniformBuffers[j];
      bufferInfoGlobal.offset = 0;
      bufferInfoGlobal.range = sizeof(UniformBufferGlobal);

      VkWriteDescriptorSet descriptorWrite = {0};
      descriptorWrite.sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet= descriptorsCtx->sets[setIndex];
      descriptorWrite.dstBinding=0;
      descriptorWrite.dstArrayElement=0;
      descriptorWrite.descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrite.descriptorCount=1;
      descriptorWrite.pBufferInfo = &bufferInfoGlobal;
      descriptorWrite.pImageInfo = NULL; // Optional
      descriptorWrite.pTexelBufferView = NULL; // Optional
      vkUpdateDescriptorSets(deviceCtx->device, 1, &descriptorWrite, 0,NULL);

     
      VkDescriptorBufferInfo bufferInfoMesh = {0};
      bufferInfoMesh.buffer = lineUniforms[i].uniformBuffers[j];
      bufferInfoMesh.offset = 0;
      bufferInfoMesh.range = sizeof(UniformBufferMesh);

      descriptorWrite.sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet= descriptorsCtx->sets[setIndex];
      descriptorWrite.dstBinding=1;
      descriptorWrite.dstArrayElement=0;
      descriptorWrite.descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrite.descriptorCount=1;
      descriptorWrite.pBufferInfo = &bufferInfoMesh;
      descriptorWrite.pImageInfo = NULL; // Optional
      descriptorWrite.pTexelBufferView = NULL; // Optional
      vkUpdateDescriptorSets(deviceCtx->device, 1, &descriptorWrite, 0,NULL);

      setIndex++;
    }
  }
  free(layouts);
}

void createDescriptorPoolsSets(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx, FrameUniforms* meshUniforms,uint32_t meshCount,FrameUniforms* lineUniforms,uint32_t gridCount, FrameUniforms* globalUniform){
  createDescriptorPool(descriptorsCtx,deviceCtx,meshCount);
  createImguiDescriptorPool(descriptorsCtx,deviceCtx);
  createDescriptorSets(descriptorsCtx,deviceCtx,meshUniforms,lineUniforms,meshCount,gridCount,globalUniform);
}


void cleanDescriptorLayout(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx){
  vkDestroyDescriptorSetLayout(deviceCtx->device,descriptorsCtx->layout,NULL);
}


void cleanupDescriptorPoolSets(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx){
  vkDestroyDescriptorPool(deviceCtx->device, descriptorsCtx->pool, NULL);
  vkDestroyDescriptorPool(deviceCtx->device, descriptorsCtx->imguiPool, NULL);
  free(descriptorsCtx->sets); 
}
