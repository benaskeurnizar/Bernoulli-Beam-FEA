#include "graphics.h"

void getTriangleAttributeDescriptions(VkVertexInputAttributeDescription* out){
  out[0].binding = 0;
  out[0].location = 0;
  out[0].format = VK_FORMAT_R32G32B32_SFLOAT; //VK_FORMAT_R32G32_SFLOAT;
  out[0].offset = offsetof(Vertex,pos);

  out[1].binding=0;
  out[1].location=1;
  out[1].format= VK_FORMAT_R32G32B32_SFLOAT;
  out[1].offset=offsetof(Vertex,color);

  out[2].binding=0;
  out[2].location=2;
  out[2].format= VK_FORMAT_R32G32B32_SFLOAT;
  out[2].offset=offsetof(Vertex,normal);
}
VkVertexInputBindingDescription getTriangleBindingDescription(){
  VkVertexInputBindingDescription bindingDescription = {0};
  bindingDescription.binding = 0;
  bindingDescription.stride = sizeof(Vertex);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return bindingDescription;
}

void getLineAttributeDescriptions(VkVertexInputAttributeDescription* out){
  out[0].binding = 0;
  out[0].location = 0;
  out[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  out[0].offset = 0;
}


VkVertexInputBindingDescription getLineBindingDescription(){
  VkVertexInputBindingDescription bindingDescription = {0};
  bindingDescription.binding = 0;
  bindingDescription.stride = sizeof(vec3);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return bindingDescription;
}
