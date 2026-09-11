#include "window.h"
#include "../fea/beam_solver.h"
#include <math.h>


void keyboard_update(World* world){
  world->input.moveForward = 0.0f;
  world->input.moveRight = 0.0f;
  if(world->input.keys & INPUT_W){
    world->input.moveForward = MOVE_SPEED*KEYBOARD_SENSITIVITY;
  }
  if(world->input.keys & INPUT_S){
    world->input.moveForward = -MOVE_SPEED*KEYBOARD_SENSITIVITY;
  }
  if(world->input.keys & INPUT_D){
    world->input.moveRight = MOVE_SPEED*KEYBOARD_SENSITIVITY;
  }
  if(world->input.keys & INPUT_A){
    world->input.moveRight = -MOVE_SPEED*KEYBOARD_SENSITIVITY;
  }
}

static void framebufferResizeCallback(GLFWwindow* window,int width,int height){
  World* world = (World*)glfwGetWindowUserPointer(window);
  world->resized = 1;   // just set a flag; actual swap chain recreation happens in drawFrame
  world->width = (uint32_t)width;
  world->height = (uint32_t)height;
  glm_perspective(glm_rad(45.0),(float)world->width/(float)world->height,0.1f,1000.0f,world->proj);
  world->proj[1][1] *= -1;
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  int press = (action==GLFW_PRESS);
  int release = (action==GLFW_RELEASE);
  World* world = (World*)glfwGetWindowUserPointer(window);
  switch(key){
  case GLFW_KEY_W:
    {
      if(press){
	world->input.keys |= INPUT_W;
      }else if(release){
	world->input.keys &= ~INPUT_W;
      }
    }break;
  case GLFW_KEY_S:
    {
      if(press){
	world->input.keys |= INPUT_S;
      }else if(release){
	world->input.keys &= ~INPUT_S;
      }
    }break;
  case GLFW_KEY_D:
    {
      if(press){
	world->input.keys |= INPUT_D;
      }else if(release){
	world->input.keys &= ~INPUT_D;
      }
    }break;
  case GLFW_KEY_A:
    {
      if(press){
	world->input.keys |= INPUT_A;
      }else if(release){
	world->input.keys &= ~INPUT_A;
      }
    }break;
  }
  keyboard_update(world);
       
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
  World* world = (World*)glfwGetWindowUserPointer(window);
  world->input.mouseDeltaX = 0.0f;
  world->input.mouseDeltaY = 0.0f;
  //double xpos, ypos;
  //glfwGetCursorPos(window, &xpos, &ypos);
  if(world->input.keys & INPUT_LEFT_MOUSE){
    if(world->input.first_mouse_press){
      world->input.mouse_xpos = xpos;
      world->input.mouse_ypos = ypos;
      world->input.first_mouse_press = 0;
      return;
    }
  
    double Dx = 0.0,Dy = 0.0;
    world->input.mouseDeltaX = xpos - world->input.mouse_xpos;
    world->input.mouseDeltaY = ypos - world->input.mouse_ypos;
    world->input.mouseDeltaX *= MOUSE_SENSITIVITY;
    world->input.mouseDeltaY *= MOUSE_SENSITIVITY;
    world->input.mouse_xpos = xpos;
    world->input.mouse_ypos = ypos;
  }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  World* world = (World*)glfwGetWindowUserPointer(window);
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
    world->input.keys |= INPUT_LEFT_MOUSE;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    world->input.mouse_xpos = xpos;
    world->input.mouse_ypos = ypos;
    world->input.first_mouse_press = 0;
  }
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
    world->input.keys &= ~INPUT_LEFT_MOUSE;
    world->input.first_mouse_press = 1;
  }
}

void initWindow(Window* window,int width,int height,World* world){
  glfwInit();
  // Because GLFW was originally designed to create an OpenGL context,we need to tell it to not create an OpenGL context with a subsequent call
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  // we will also tell it to not handle resizable windows for now to reduce overhead
  //glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);

  window->window = glfwCreateWindow(width,height, "Vulkanwindow Organized Triangle", NULL, NULL);
  glfwSetWindowUserPointer(window->window, world);   // <-- stash the app pointer on the window
  glfwSetFramebufferSizeCallback(window->window,framebufferResizeCallback);

  glfwSetKeyCallback(window->window, keyCallback);
  glfwSetCursorPosCallback(window->window, cursor_position_callback);
  glfwSetMouseButtonCallback(window->window, mouse_button_callback);
}

int window_should_close(Window* window){
  return glfwWindowShouldClose(window->window);
}


void window_poll_events(){
  glfwPollEvents();
}

void window_destroy(Window* window){
  glfwDestroyWindow(window->window);
  glfwTerminate();
}


/*


  	printf("Elem1 xi=0: theta_x=%.6f theta_y=%.6f theta_z=%.6f x=%.6f y=%.6f z=%.6f\n",
	       get_x_rotation(&world->beams[0], 0.0f, world->beams[0].local_dis[3], world->beams[0].local_dis[9]),
	       get_y_rotation(&world->beams[0], 0.0f, world->beams[0].local_dis[1], world->beams[0].local_dis[5], world->beams[0].local_dis[7], world->beams[0].local_dis[11]),
	       get_z_rotation(&world->beams[0], 0.0f, world->beams[0].local_dis[2], world->beams[0].local_dis[4], world->beams[0].local_dis[8], world->beams[0].local_dis[10]),
	       get_x_displacement(&world->beams[0], 0.0f, world->beams[0].local_dis[0], world->beams[0].local_dis[6]),
	       get_y_displacement(&world->beams[0], 0.0f, world->beams[0].local_dis[1], world->beams[0].local_dis[5], world->beams[0].local_dis[7], world->beams[0].local_dis[11]),
	       get_z_displacement(&world->beams[0], 0.0f, world->beams[0].local_dis[2], world->beams[0].local_dis[4], world->beams[0].local_dis[8], world->beams[0].local_dis[10]));

	printf("Elem1 xi=1: theta_x=%.6f theta_y=%.6f theta_z=%.6f x=%.6f y=%.6f z=%.6f\n",
	       get_x_rotation(&world->beams[0], 1.0f, world->beams[0].local_dis[3], world->beams[0].local_dis[9]),
	       get_y_rotation(&world->beams[0], 1.0f, world->beams[0].local_dis[1], world->beams[0].local_dis[5], world->beams[0].local_dis[7], world->beams[0].local_dis[11]),
	       get_z_rotation(&world->beams[0], 1.0f, world->beams[0].local_dis[2], world->beams[0].local_dis[4], world->beams[0].local_dis[8], world->beams[0].local_dis[10]),
	       get_x_displacement(&world->beams[0], 1.0f, world->beams[0].local_dis[0], world->beams[0].local_dis[6]),
	       get_y_displacement(&world->beams[0], 1.0f, world->beams[0].local_dis[1], world->beams[0].local_dis[5], world->beams[0].local_dis[7], world->beams[0].local_dis[11]),
	       get_z_displacement(&world->beams[0], 1.0f, world->beams[0].local_dis[2], world->beams[0].local_dis[4], world->beams[0].local_dis[8], world->beams[0].local_dis[10]));

	printf("Elem2 xi=0: theta_x=%.6f theta_y=%.6f theta_z=%.6f x=%.6f y=%.6f z=%.6f\n",
	       get_x_rotation(&world->beams[1], 0.0f, world->beams[1].local_dis[3], world->beams[1].local_dis[9]),
	       get_y_rotation(&world->beams[1], 0.0f, world->beams[1].local_dis[1], world->beams[1].local_dis[5], world->beams[1].local_dis[7], world->beams[1].local_dis[11]),
	       get_z_rotation(&world->beams[1], 0.0f, world->beams[1].local_dis[2], world->beams[1].local_dis[4], world->beams[1].local_dis[8], world->beams[1].local_dis[10]),
	       get_x_displacement(&world->beams[1], 0.0f, world->beams[1].local_dis[0], world->beams[1].local_dis[6]),
	       get_y_displacement(&world->beams[1], 0.0f, world->beams[1].local_dis[1], world->beams[1].local_dis[5], world->beams[1].local_dis[7], world->beams[1].local_dis[11]),
	       get_z_displacement(&world->beams[1], 0.0f, world->beams[1].local_dis[2], world->beams[1].local_dis[4], world->beams[1].local_dis[8], world->beams[1].local_dis[10]));

	printf("Elem2 xi=1: theta_x=%.6f theta_y=%.6f theta_z=%.6f x=%.6f y=%.6f z=%.6f\n",
	       get_x_rotation(&world->beams[1], 1.0f, world->beams[1].local_dis[3], world->beams[1].local_dis[9]),
	       get_y_rotation(&world->beams[1], 1.0f, world->beams[1].local_dis[1], world->beams[1].local_dis[5], world->beams[1].local_dis[7], world->beams[1].local_dis[11]),
	       get_z_rotation(&world->beams[1], 1.0f, world->beams[1].local_dis[2], world->beams[1].local_dis[4], world->beams[1].local_dis[8], world->beams[1].local_dis[10]),
	       get_x_displacement(&world->beams[1], 1.0f, world->beams[1].local_dis[0], world->beams[1].local_dis[6]),
	       get_y_displacement(&world->beams[1], 1.0f, world->beams[1].local_dis[1], world->beams[1].local_dis[5], world->beams[1].local_dis[7], world->beams[1].local_dis[11]),
	       get_z_displacement(&world->beams[1], 1.0f, world->beams[1].local_dis[2], world->beams[1].local_dis[4], world->beams[1].local_dis[8], world->beams[1].local_dis[10]));

 */
