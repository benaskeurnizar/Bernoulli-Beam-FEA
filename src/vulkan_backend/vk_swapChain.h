#pragma once
#include "vk_types.h"
#include "vk_device.h"
#include "../platform/window.h"

typedef struct{
  VkImage colorImage;
  VkImageView colorImageView;
  VkDeviceMemory colorImageMemory;
  VkFormat format;
}MsaaColorCtx;

typedef struct{
  VkSwapchainKHR swapChain;
  VkImage* swapChainImages;
  VkImageView* swapChainImageViews;
  uint32_t imageCount;
  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;
  VkFormat depthImageFormat;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  MsaaColorCtx msaaCtx;
}VkSwapchainCtx;


void createSwapChainCtx(VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx,Window* window,VkSurfaceKHR surface);
void cleanupSwapChainCtx(VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx);
