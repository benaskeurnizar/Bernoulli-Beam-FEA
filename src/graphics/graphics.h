/*#pragma once

#include "cglm.h"
#include <vulkan/vulkan.h>
//#include "../platform/window.h"
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "../fea/fea_types.h"*/

#pragma once
#include "cglm.h"

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>          // this pulls in vulkan.h correctly, with the right macros already set

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "../fea/fea_types.h"
#include "math.h"
#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

typedef enum {
  // Deformed Shape
  SHOW_DEFORM_X       = 1 << 0,
  SHOW_DEFORM_Y       = 1 << 1,
  SHOW_DEFORM_Z       = 1 << 2,
  SHOW_ROT_X          = 1 << 3,
  SHOW_ROT_Y          = 1 << 4,
  SHOW_ROT_Z          = 1 << 5,

  // Stresses
  SHOW_AXIAL_STRESS   = 1 << 6,
  SHOW_BEND_STRESS_Y  = 1 << 7,
  SHOW_BEND_STRESS_Z  = 1 << 8,
  SHOW_SHEAR_STRESS   = 1 << 9,
  SHOW_VON_MISES      = 1 << 10,

  // Diagrams
  SHOW_AXIAL_FORCE    = 1 << 11,
  SHOW_SHEAR_VY       = 1 << 12,
  SHOW_SHEAR_VZ       = 1 << 13,
  SHOW_MOMENT_MY      = 1 << 14,
  SHOW_MOMENT_MZ      = 1 << 15,
  SHOW_TORQUE_MX      = 1 << 16,
} RenderFlags;

typedef struct Input {
  // Button/key state
  uint32_t keys;

  // Current mouse positions
  double mouse_xpos,mouse_ypos;
  int first_mouse_press;
  
  // Mouse movement this frame
  double mouseDeltaX;
  double mouseDeltaY;

  // Movement input this frame (-1..1)
  float moveForward;
  float moveRight;
} InputState;


typedef struct{
  vec3 pos;
  vec3 color;
  vec3 normal;
}Vertex;

typedef struct{
  float u,v;
}point2D;

typedef struct {
  float position[3];
} LineVertex;

typedef struct {
  LineVertex* vertices;
  uint32_t vertexCount;
} Grid;


typedef struct{
  mat4 view;
  mat4 proj;
  vec3 cam_pos;
}UniformBufferGlobal;

typedef struct{
  mat4 model;
}UniformBufferMesh;

typedef struct {
  Vertex* vertices;
  uint32_t vertexCount;

  uint32_t* indices;
  uint32_t indexCount;
  mat4 model;

  int updated;
} Mesh;

typedef struct{
  vec3 pos;
  vec3 lookdir;
  vec3 up;
  vec3 right;
  mat4 view;
  float yaw;
  float pitch;
}Camera;

typedef struct {
  Camera camera;
  InputState input;
  mat4 proj;
  int width;
  int height;
  Mesh* meshes;
  uint32_t meshCount;
  uint32_t resized;

  Grid* grids;
  int gridCount;
  
  int update_beam_buffers;
  int new_grid;

  uint32_t render_flags;
} World;

typedef enum {
  DIAGRAM_N,
  DIAGRAM_VY,
  DIAGRAM_VZ,
  DIAGRAM_T,
  DIAGRAM_MY,
  DIAGRAM_MZ
} DIAGRAM_TYPE;

typedef struct {
  LineVertex *vertices;
  uint32_t vertexCount;
} Diagram;

VkVertexInputBindingDescription getBindingDescription();
void getAttributeDescriptions(VkVertexInputAttributeDescription* out);

void createGridLines(Grid* grid,int halfSize, float spacing);
void initWorld(World* world, uint32_t width, uint32_t height);
  
void updateWorld(World* world,float deltaTime);


void getTriangleAttributeDescriptions(VkVertexInputAttributeDescription* out);
VkVertexInputBindingDescription getTriangleBindingDescription();

void getLineAttributeDescriptions(VkVertexInputAttributeDescription* out);

VkVertexInputBindingDescription getLineBindingDescription();


