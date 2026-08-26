#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "../graphics/graphics.h"
#include "../graphics/draw_elements.h"
#include "../graphics/section_outline.h"

#define KEYBOARD_SENSITIVITY 0.05f
#define MOUSE_SENSITIVITY 0.1f
#define MOVE_SPEED 4.0f

typedef enum{
  INPUT_W           = 1u << 0,
  INPUT_A           = 1u << 1,
  INPUT_S           = 1u << 2,
  INPUT_D           = 1u << 3,
  INPUT_SPACE       = 1u << 4,
  INPUT_SHIFT       = 1u << 5,
  INPUT_LEFT_MOUSE  = 1u << 6,
  INPUT_RIGHT_MOUSE = 1u << 7,
}InputKeys;


typedef struct{
   GLFWwindow * window;
   int width;
   int height;
}Window;

void initWindow(Window* window,int width,int height,World* world);

int window_should_close(Window* window);

void window_poll_events();
void window_destroy(Window* window);
