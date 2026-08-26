#include "vk_swapChain.h"

VkFormat findDepthFormat(void){
    return VK_FORMAT_D32_SFLOAT;
}

uint32_t clampUint32(uint32_t value,
                     uint32_t min,
                     uint32_t max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(VkSurfaceFormatKHR* availableFormats,uint32_t availableFormatsCount){
  for(int i=0;i<(int)availableFormatsCount;i++){
    if(availableFormats[i].format == VK_FORMAT_R8G8B8A8_SRGB && availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ){
      return availableFormats[i];
    }
  }
  return availableFormats[0];
}


VkPresentModeKHR chooseSwapPresentMode(VkPresentModeKHR* availablePresentModes,uint32_t availablePresentModesCount){
  for(int i=0;i<(int)availablePresentModesCount;i++){
    if(availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR){
      return availablePresentModes[i];
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR ;
}

VkExtent2D chooseSwapExtent(Window* window,const VkSurfaceCapabilitiesKHR capabilities){
  if(capabilities.currentExtent.width != UINT32_MAX){
     return capabilities.currentExtent;
  }else{
    int width,height;
    glfwGetFramebufferSize(window->window,&width,&height);
    VkExtent2D actualExtent = {0};
    actualExtent.width = width;
    actualExtent.height = height;
    actualExtent.width = clampUint32(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = clampUint32(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actualExtent;
  }
}

void createMsaaColorResources(VkSwapchainCtx* swapChainCtx, VkDeviceCtx* deviceCtx)
{
  //NOTE: CREATE THE DEPTH IMAGE
  VkFormat colorFormat = swapChainCtx->swapChainImageFormat;

  VkImageCreateInfo imageInfo = {0};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = swapChainCtx->swapChainExtent.width;
  imageInfo.extent.height = swapChainCtx->swapChainExtent.height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = colorFormat;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  imageInfo.samples = VK_SAMPLE_COUNT_4_BIT; // the key difference from a normal color image
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(deviceCtx->device,
		    &imageInfo,
		    NULL,
		    &swapChainCtx->msaaCtx.colorImage) != VK_SUCCESS) {
    fprintf(stderr, "failed to create depth image!\n");
    exit(EXIT_FAILURE);
  }
  
  //NOTE: CREATE DEPTH MEMORY
  VkMemoryRequirements memRequirements;

  vkGetImageMemoryRequirements(
			       deviceCtx->device,
			       swapChainCtx->msaaCtx.colorImage,
			       &memRequirements
			       );
  VkMemoryAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
    findMemoryType(
		   deviceCtx,
		   memRequirements.memoryTypeBits,
		   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		   );

  if (vkAllocateMemory(
		       deviceCtx->device,
		       &allocInfo,
		       NULL,
		       &swapChainCtx->msaaCtx.colorImageMemory) != VK_SUCCESS) {
    fprintf(stderr, "failed to allocate depth image memory!\n");
    exit(EXIT_FAILURE);
  }
  //NOTE: BIND DEPTH MEMORY
  if (vkBindImageMemory(
			deviceCtx->device,
			swapChainCtx->msaaCtx.colorImage,
			swapChainCtx->msaaCtx.colorImageMemory,
			0) != VK_SUCCESS) {
    fprintf(stderr, "failed to bind depth image memory!\n");
    exit(EXIT_FAILURE);
  }

  //NOTE: CREATE DEPTH IMAGE VIEW
  VkImageViewCreateInfo createInfo = {0};
  createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  createInfo.image = swapChainCtx->msaaCtx.colorImage;
  createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  createInfo.format = colorFormat;


  createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  createInfo.subresourceRange.baseMipLevel = 0;
  createInfo.subresourceRange.levelCount = 1;
  createInfo.subresourceRange.baseArrayLayer = 0;
  createInfo.subresourceRange.layerCount = 1;

  if(vkCreateImageView(deviceCtx->device,&createInfo,NULL,&swapChainCtx->msaaCtx.colorImageView)!=VK_SUCCESS){
    fprintf(stderr,"failed to create image views! \n");
    exit(EXIT_FAILURE);
  }
}


void createDepthResources(VkSwapchainCtx* swapChainCtx, VkDeviceCtx* deviceCtx)
{
  //NOTE: CREATE THE DEPTH IMAGE
  VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;

  VkImageCreateInfo imageInfo = {0};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = swapChainCtx->swapChainExtent.width;
  imageInfo.extent.height = swapChainCtx->swapChainExtent.height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = depthFormat;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
  imageInfo.samples = VK_SAMPLE_COUNT_4_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(deviceCtx->device,
		    &imageInfo,
		    NULL,
		    &swapChainCtx->depthImage) != VK_SUCCESS) {
    fprintf(stderr, "failed to create depth image!\n");
    exit(EXIT_FAILURE);
  }
  
  //NOTE: CREATE DEPTH MEMORY
  VkMemoryRequirements memRequirements;

  vkGetImageMemoryRequirements(
			       deviceCtx->device,
			       swapChainCtx->depthImage,
			       &memRequirements
			       );
  VkMemoryAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
    findMemoryType(
		   deviceCtx,
		   memRequirements.memoryTypeBits,
		   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		   );

  if (vkAllocateMemory(
		       deviceCtx->device,
		       &allocInfo,
		       NULL,
		       &swapChainCtx->depthImageMemory) != VK_SUCCESS) {
    fprintf(stderr, "failed to allocate depth image memory!\n");
    exit(EXIT_FAILURE);
  }
  //NOTE: BIND DEPTH MEMORY
  if (vkBindImageMemory(
			deviceCtx->device,
			swapChainCtx->depthImage,
			swapChainCtx->depthImageMemory,
			0) != VK_SUCCESS) {
    fprintf(stderr, "failed to bind depth image memory!\n");
    exit(EXIT_FAILURE);
  }

  //NOTE: CREATE DEPTH IMAGE VIEW
  VkImageViewCreateInfo createInfo = {0};
  createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  createInfo.image = swapChainCtx->depthImage;
  createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  createInfo.format = depthFormat;


  createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  createInfo.subresourceRange.baseMipLevel = 0;
  createInfo.subresourceRange.levelCount = 1;
  createInfo.subresourceRange.baseArrayLayer = 0;
  createInfo.subresourceRange.layerCount = 1;

  if(vkCreateImageView(deviceCtx->device,&createInfo,NULL,&swapChainCtx->depthImageView)!=VK_SUCCESS){
    fprintf(stderr,"failed to create image views! \n");
    exit(EXIT_FAILURE);
  }
  swapChainCtx->depthImageFormat = depthFormat;
}


//NOTE: creating the swap chain
void createSwapChain(VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx,Window* window,VkSurfaceKHR surface){
  SwapChainSupportDetails swapChainSupport = querySwapChainSupport(surface,deviceCtx->physicalDevice);
  VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats,swapChainSupport.formatsCount);
  VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes,swapChainSupport.presentModesCount);
  VkExtent2D extent = chooseSwapExtent(window,swapChainSupport.capabilities);//TODO

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount+1;
  if(swapChainSupport.capabilities.maxImageCount>0 && imageCount > swapChainSupport.capabilities.maxImageCount){
    imageCount=swapChainSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo = {0};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;
  createInfo.minImageCount=imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  QueueFamilyIndices indices = findQueueFamilies(surface,deviceCtx->physicalDevice);
  uint32_t queueFamilyIndices[] = {indices.graphicsFamily,indices.presentFamily};
  if (indices.graphicsFamily != indices.presentFamily) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  }else{
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0; // Optional
    createInfo.pQueueFamilyIndices = NULL; // Optional
  }
  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;
  if (vkCreateSwapchainKHR(deviceCtx->device, &createInfo,NULL, &(swapChainCtx->swapChain)) != VK_SUCCESS){
    fprintf(stderr,"failed to create swap chain! \n");
    exit(EXIT_FAILURE);
  }

  vkGetSwapchainImagesKHR(deviceCtx->device,swapChainCtx->swapChain, &(swapChainCtx->imageCount),NULL);
  swapChainCtx->swapChainImages = (VkImage*)calloc((int)swapChainCtx->imageCount,sizeof(VkImage));
  vkGetSwapchainImagesKHR(deviceCtx->device, swapChainCtx->swapChain, &(swapChainCtx->imageCount),swapChainCtx->swapChainImages);

  swapChainCtx->swapChainImageFormat = surfaceFormat.format;
  swapChainCtx->swapChainExtent= extent;
  createDepthResources(swapChainCtx, deviceCtx);
}


void createImageViews(VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx){
  swapChainCtx->swapChainImageViews = (VkImageView*)calloc(swapChainCtx->imageCount,sizeof(VkImageView));

  for(uint32_t i=0;i< swapChainCtx->imageCount;i++){
    VkImageViewCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = swapChainCtx->swapChainImages[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = swapChainCtx->swapChainImageFormat;

    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if(vkCreateImageView(deviceCtx->device,&createInfo,NULL,&(swapChainCtx->swapChainImageViews[i]))!=VK_SUCCESS){
       fprintf(stderr,"failed to create image views! \n");
       exit(EXIT_FAILURE);
    }
    
  }
}

void createSwapChainCtx(VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx,Window* window,VkSurfaceKHR surface){
  createSwapChain(swapChainCtx,deviceCtx,window,surface);
  createImageViews(swapChainCtx,deviceCtx);
  createMsaaColorResources(swapChainCtx,deviceCtx);
}


void cleanupSwapChainCtx(VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx){
  for (uint32_t i = 0; i < swapChainCtx->imageCount; i++){
    vkDestroyImageView(deviceCtx->device,swapChainCtx->swapChainImageViews[i],NULL);
  }
  vkDestroySwapchainKHR(deviceCtx->device, swapChainCtx->swapChain, NULL);
  vkDestroyImageView(deviceCtx->device, swapChainCtx->depthImageView, NULL);
  vkDestroyImage(deviceCtx->device, swapChainCtx->depthImage, NULL);
  vkFreeMemory(deviceCtx->device, swapChainCtx->depthImageMemory, NULL);

  vkDestroyImageView(deviceCtx->device, swapChainCtx->msaaCtx.colorImageView, NULL);
  vkDestroyImage(deviceCtx->device, swapChainCtx->msaaCtx.colorImage, NULL);
  vkFreeMemory(deviceCtx->device, swapChainCtx->msaaCtx.colorImageMemory, NULL);
}


