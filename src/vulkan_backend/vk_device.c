#include "vk_device.h"

QueueFamilyIndices findQueueFamilies(VkSurfaceKHR surface,VkPhysicalDevice device) {
  QueueFamilyIndices indices = {0};

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device,&queueFamilyCount,NULL);

  VkQueueFamilyProperties* queueFamilies = (VkQueueFamilyProperties*)calloc(queueFamilyCount,sizeof(VkQueueFamilyProperties));
  vkGetPhysicalDeviceQueueFamilyProperties(device,&queueFamilyCount,queueFamilies);
  for(uint32_t i=0;i<queueFamilyCount;i++){
    if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT){     
      indices.graphicsFamily = i;
      indices.graphicsFamilyHasValue = true;      
    }
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device,i,surface,&presentSupport);
    if(presentSupport){
      indices.presentFamily = i;
      indices.presentFamilyHasValue = true;
    }
    if(indices.graphicsFamilyHasValue && indices.presentFamilyHasValue){
      break;
    }
  }
  return indices;
}

SwapChainSupportDetails querySwapChainSupport(VkSurfaceKHR surface,VkPhysicalDevice device){
  SwapChainSupportDetails details =  {0};
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,surface,&(details.capabilities));
  //NOTE: querying the supported surface formats
  vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface,&(details.formatsCount),NULL);
  if(details.formatsCount != 0){
    details.formats =  (VkSurfaceFormatKHR*)calloc((int)details.formatsCount,sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface,&(details.formatsCount),details.formats);
  }

  vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&(details.presentModesCount),NULL);
  if(details.presentModesCount != 0){
    details.presentModes = (VkPresentModeKHR*)calloc((int)details.presentModesCount,sizeof(VkPresentModeKHR));
    vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&(details.presentModesCount),details.presentModes);
  }
  return details;
}




bool checkDeviceExtensionSupport(VkPhysicalDevice device){
  uint32_t extensionsCount;
  vkEnumerateDeviceExtensionProperties(device,NULL,&extensionsCount,NULL);

  VkExtensionProperties* availableExtensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties)*extensionsCount);
  vkEnumerateDeviceExtensionProperties(device,NULL,&extensionsCount,availableExtensions);

  int count = deviceExtensionsCount;
  for(uint32_t i=0;i<extensionsCount;i++){
    for(int j=0;j< deviceExtensionsCount;j++){
      if(strcmp(deviceExtensions[j],availableExtensions[i].extensionName)==0){
	count--;
	break;
      }
    }
  }
  free(availableExtensions);   // <-- add this before returning
  return count <=0;
}


bool isDeviceSuitable(VkSurfaceKHR surface,VkPhysicalDevice device){
  QueueFamilyIndices indices = findQueueFamilies(surface,device);
  bool extensionsSupported = checkDeviceExtensionSupport(device);

  bool swapChainAdequate = false;
  if(extensionsSupported){
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(surface,device);
    swapChainAdequate = !(swapChainSupport.formatsCount == 0) && !(swapChainSupport.presentModesCount == 0) ;
  }
  
  return indices.graphicsFamilyHasValue && extensionsSupported && swapChainAdequate;
}

void pickPhysicalDevice(VkInstanceCtx* Instance,VkDeviceCtx* Device,VkSurfaceKHR surface){
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(Instance->instance,&deviceCount,NULL);
  if(deviceCount == 0){
    fprintf(stderr,"failed to find GPUs with Vulkan support! \n");
    exit(EXIT_FAILURE);
  }
  VkPhysicalDevice* devices = (VkPhysicalDevice*)calloc(deviceCount,sizeof(VkPhysicalDevice));
  vkEnumeratePhysicalDevices(Instance->instance,&deviceCount,devices);
  for(uint32_t i=0;i<deviceCount;i++){
    if(isDeviceSuitable(surface,devices[i])){
      Device->physicalDevice = devices[i];
      break;
    }
  }
  if(Device->physicalDevice == VK_NULL_HANDLE){
    fprintf(stderr,"failed to find GPUs with Vulkan support! \n");
    exit(EXIT_FAILURE);
  }
  
}


void createLogicalDevice(VkDeviceCtx* Device,VkSurfaceKHR surface){
  QueueFamilyIndices indices = findQueueFamilies(surface,Device->physicalDevice);

  int allocation_size = 0;
  uint32_t uniqueQueueFamilies[2];
  if(indices.graphicsFamilyHasValue && indices.presentFamilyHasValue){
    if(indices.graphicsFamily == indices.presentFamily){
      allocation_size = 1;
      uniqueQueueFamilies[0] = indices.graphicsFamily;
    }else{
      allocation_size = 2;
      uniqueQueueFamilies[0] = indices.graphicsFamily;
      uniqueQueueFamilies[1] = indices.presentFamily;
    }
  }else{
    fprintf(stderr,"IDK, something maybe wrong?? \n");
    exit(EXIT_FAILURE);
  }
  VkDeviceQueueCreateInfo* queueCreateInfos = (VkDeviceQueueCreateInfo*)malloc(sizeof(VkDeviceQueueCreateInfo)*allocation_size);
  float queuePriority = 1.0f;
  for(int i=0;i<allocation_size;i++){
    memset(&queueCreateInfos[i], 0, sizeof(VkDeviceQueueCreateInfo));
    queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;//VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfos[i].queueFamilyIndex = uniqueQueueFamilies[i];
    queueCreateInfos[i].queueCount = 1;
    queueCreateInfos[i].pQueuePriorities = &queuePriority;
  }

  VkPhysicalDeviceFeatures deviceFeatures = {0};

  VkDeviceCreateInfo createInfo = {0};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = &(queueCreateInfos[0]);
  createInfo.queueCreateInfoCount = allocation_size;
  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount = deviceExtensionsCount;
  createInfo.ppEnabledExtensionNames = deviceExtensions;
  /* if(enableValidationLayers){
    createInfo.enabledLayerCount = validationLayerCount;
    createInfo.ppEnabledLayerNames = validationLayers;
  }else{
    createInfo.enabledLayerCount = 0;
    }*/

  if(vkCreateDevice(Device->physicalDevice,&createInfo,NULL,&(Device->device)) != VK_SUCCESS){
    fprintf(stderr,"failed to create logical device! \n");
    exit(EXIT_FAILURE);
  }
  printf("logical device created successfully!\n");
  vkGetDeviceQueue(Device->device, indices.graphicsFamily, 0, &(Device->graphicsQueue));
  vkGetDeviceQueue(Device->device, indices.presentFamily, 0, &(Device->presentQueue));
}

void createDeviceCtx(VkInstanceCtx* Instance,VkDeviceCtx* Device,VkSurfaceKHR surface){
  pickPhysicalDevice(Instance,Device,surface);
  createLogicalDevice(Device,surface);
}

void cleanupDeviceCtx(VkDeviceCtx* Device){
  vkDestroyDevice(Device->device,NULL);;
}
