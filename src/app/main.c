#include "../platform/window.h"
#include "../renderer/renderer.h"
#include "../renderer/draw.h"

int main(){
  Window window ;
  Renderer renderer;
  World world = {0};
  world.input.first_mouse_press = 1;
  //TODO: create window and init renderer
  initWorld(&world, 800, 600);
  initWindow(&window,800,600,&world);
  initRenderer(&renderer,&window,&world);
  float lastTime = (float)glfwGetTime();
  while(!window_should_close(&window)){
    //TODO: update and render framex
    float currentTime = (float)glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    window_poll_events();
    updateWorld(&world,deltaTime);
    drawFrame(&renderer,&window,&world);
    world.input.mouseDeltaX = 0.0;
    world.input.mouseDeltaY = 0.0;

    world.input.moveForward = 0.0f;
    world.input.moveRight = 0.0f;
  }
  //TODO: clean up and shut down
  cleanupRenderer(&renderer,&world);
  window_destroy(&window);
  cleanupWorld(&world);
  
  return 0;
}
