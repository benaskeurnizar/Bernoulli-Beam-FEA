#pragma once

#include "vk_types.h"
#include "vk_instance.h"

typedef struct{
  VkPhysicalDevice physicalDevice;
  VkDevice device;
  VkQueue graphicsQueue;
  VkQueue presentQueue;
}VkDeviceCtx;

typedef struct{
  uint32_t graphicsFamily;
  bool graphicsFamilyHasValue;   // manual "has_value" flag
  uint32_t presentFamily;
  bool presentFamilyHasValue;
}QueueFamilyIndices;

typedef struct{
  VkSurfaceCapabilitiesKHR capabilities;
  VkSurfaceFormatKHR* formats;
  uint32_t formatsCount;
  VkPresentModeKHR* presentModes;
  uint32_t presentModesCount;
}SwapChainSupportDetails;


void createDeviceCtx(VkInstanceCtx* Instance,VkDeviceCtx* Device,VkSurfaceKHR surface);
QueueFamilyIndices findQueueFamilies(VkSurfaceKHR surface,VkPhysicalDevice device);
bool checkDeviceExtensionSupport(VkPhysicalDevice device);
SwapChainSupportDetails querySwapChainSupport(VkSurfaceKHR surface,VkPhysicalDevice device);
void cleanupDeviceCtx(VkDeviceCtx* Device);
