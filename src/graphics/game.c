#include "graphics.h"
#include "../fea/beam.h"
#include "draw_elements.h"


void initCamera(World* world){
  memset(&(world->camera), 0, sizeof(world->camera));
  world->camera.pos[0] = 2.0;
  world->camera.pos[1] = 1.0;
  world->camera.pos[2] = 0.5;
  vec3 worldUP;
  worldUP[0] = 0.0;
  worldUP[1] = 0.0;
  worldUP[2] = 1.0;

  vec3 origin;
  glm_vec_zero(origin);
  glm_vec_sub(origin,world->camera.pos,world->camera.lookdir);
  glm_vec_normalize(world->camera.lookdir);

  glm_vec_cross(world->camera.lookdir,worldUP,world->camera.right);
  glm_vec_normalize(world->camera.right);

  glm_vec_cross(world->camera.right,world->camera.lookdir,world->camera.up);
  glm_vec_normalize(world->camera.up);

  glm_lookat(world->camera.pos,origin,world->camera.up, world->camera.view);
  world->camera.pitch = glm_deg(asinf(world->camera.lookdir[2]));
  world->camera.yaw   = glm_deg(atan2f(world->camera.lookdir[2], world->camera.lookdir[0]));

}


void updateCamera(World* world,float deltaTime){
  vec3 worldUP;
  worldUP[0] = 0.0;
  worldUP[1] = 0.0;
  worldUP[2] = 1.0;
  
  world->camera.yaw   += world->input.mouseDeltaX;
  world->camera.pitch -= world->input.mouseDeltaY;   // subtract because +Y is down

  if (world->camera.pitch > 89.0f)  world->camera.pitch = 89.0f;
  if (world->camera.pitch < -89.0f) world->camera.pitch = -89.0f;

  float yawRad   = glm_rad(world->camera.yaw);
  float pitchRad = glm_rad(world->camera.pitch);

  world->camera.lookdir[0] = cosf(yawRad)*cosf(pitchRad);
  world->camera.lookdir[2] = sinf(pitchRad);
  world->camera.lookdir[1] = sinf(yawRad)*cosf(pitchRad);
  glm_vec_normalize(world->camera.lookdir);

  glm_vec_cross(world->camera.lookdir,worldUP,world->camera.right);
  glm_vec_normalize(world->camera.right);

  glm_vec_cross(world->camera.right,world->camera.lookdir,world->camera.up);
  glm_vec_normalize(world->camera.up);

  vec3 forward;
  glm_vec_scale(world->camera.lookdir,world->input.moveForward*deltaTime,forward);
  glm_vec_add(world->camera.pos,forward,world->camera.pos);

  vec3 right;
  glm_vec_scale(world->camera.right,world->input.moveRight*deltaTime,right);
  glm_vec_add(world->camera.pos,right,world->camera.pos);
  
  vec3 target;
  glm_vec_zero(target);
  glm_vec_add(world->camera.pos,world->camera.lookdir,target);

  glm_lookat(world->camera.pos,target,world->camera.up, world->camera.view);
}

void updateWorld(World* world,float deltaTime){
  updateCamera(world,deltaTime);
  double time = glfwGetTime();
  /*vec3 axis;
  axis[0] = 0.0;
  axis[1] = 0.0;
  axis[2] = 1.0;
  for(uint32_t i=0;i<world->meshCount;i++){
    glm_mat4_identity(world->meshes[i].model);
    glm_rotate(world->meshes[i].model,time*glm_rad(90.0),axis);
    }*/
}


void initWorld(World* world, uint32_t width, uint32_t height)
{
  memset(world, 0, sizeof(World));

  world->width = width;
  world->height = height;

  glm_perspective(glm_rad(45.0f),
		  (float)width / (float)height,
		  0.1f,
		  1000.0f,
		  world->proj);

  world->proj[1][1] *= -1.0f;

  initCamera(world);
  world->gridCount = 1;
  world->grids = (Grid*)calloc(world->gridCount,sizeof(Grid));
  createGridLines(&(world->grids[0]), 100, 1.0f);
  
}

void cleanupWorld(World* world)
{
  for (uint32_t i = 0; i < world->meshCount; i++) {
    free(world->meshes[i].vertices);
    free(world->meshes[i].indices);
  }

  free(world->meshes);
}
