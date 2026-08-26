#pragma once
#include "vk_types.h"


typedef struct{
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
}VkInstanceCtx;


void createInstanceCtx(VkInstanceCtx* instanceCtx);


void cleanupInstanceCtx(VkInstanceCtx* instanceCtx);
