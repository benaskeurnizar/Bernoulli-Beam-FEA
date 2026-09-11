#include "../platform/window.h"
#include "../renderer/renderer.h"
#include "../renderer/draw.h"
#include "../parser/parse_fea.h"
#include "../fea/beam.h"
int main(){
  Window window ;
  Renderer renderer;
  World world = {0};
  FeaModel feaModel = {0};

  initPaths();
  
  world.input.first_mouse_press = 1;
  //TODO: create window and init renderer
  initWorld(&world, 800, 600);
  initFea(&feaModel);
  //NOTE: after the first upload of the fea model,the meshes should be created
  //NOTE: FOR EACH BEAM,CREATE A MESH FOR IT
  world.meshCount = feaModel.num_beam_elements;
  world.meshes = (Mesh*)calloc(feaModel.num_beam_elements,sizeof(Mesh));
  for(int i=0;i<feaModel.num_beam_elements;i++){
    createElementVertices(&feaModel,&(feaModel.beams[i]),&(world.meshes[i]),world.render_flags);
  }
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
    drawFrame(&renderer,&window,&world,&feaModel);
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
