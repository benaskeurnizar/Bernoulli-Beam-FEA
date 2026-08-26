#include "graphics.h"
#include "../vulkan_backend/vk_buffer.h"
#include "../fea/beam_result.h"

float sigma_min = -0.05e6;
float sigma_max =  0.05e6;

static void addLine(LineVertex* vertices,
                    uint32_t* index,
                    float x1, float y1,
                    float x2, float y2)
{
    vertices[*index].position[0] = x1;
    vertices[*index].position[1] = y1;
    vertices[*index].position[2] = 0.0f;
    (*index)++;

    vertices[*index].position[0] = x2;
    vertices[*index].position[1] = y2;
    vertices[*index].position[2] = 0.0f;
    (*index)++;
}


void createGridLines(Grid* grid,int halfSize, float spacing){
  grid->vertexCount = 2 * 2 * (2 * halfSize + 1);   // two line sets, not one
  grid->vertices = (LineVertex*)malloc(grid->vertexCount * sizeof(LineVertex));

  if (!grid->vertices) {
    fprintf(stderr, "failed to allocate grid vertices\n");
    exit(EXIT_FAILURE);
  }

  uint32_t i = 0;

  float min = -halfSize * spacing;
  float max =  halfSize * spacing;

  /* Lines parallel to X */
  int index = 0;
  for(int y=-halfSize;y<=halfSize;y++){
    float yy = y*spacing;

    addLine(grid->vertices, &index,
                min, yy,
                max, yy);
  }

  /* Lines parallel to Y */
  for(int x=-halfSize;x<=halfSize;x++){
    float xx = x*spacing;
    addLine(grid->vertices, &index,
                xx, min,
                xx, max);
  }
}

void createCubeMesh(Mesh* mesh){
  const Vertex vertices[] = {
    // Front  (+Z)
    {{-0.5f,-0.5f, 0.5f},{1,0,0},{0,0,1}},
    {{ 0.5f,-0.5f, 0.5f},{0,1,0},{0,0,1}},
    {{ 0.5f, 0.5f, 0.5f},{0,0,1},{0,0,1}},
    {{-0.5f, 0.5f, 0.5f},{1,1,1},{0,0,1}},
    // Back   (-Z)
    {{ 0.5f,-0.5f,-0.5f},{1,0,0},{0,0,-1}},
    {{-0.5f,-0.5f,-0.5f},{0,1,0},{0,0,-1}},
    {{-0.5f, 0.5f,-0.5f},{0,0,1},{0,0,-1}},
    {{ 0.5f, 0.5f,-0.5f},{1,1,1},{0,0,-1}},
    // Right  (+X)
    {{ 0.5f,-0.5f, 0.5f},{1,0,0},{1,0,0}},
    {{ 0.5f,-0.5f,-0.5f},{0,1,0},{1,0,0}},
    {{ 0.5f, 0.5f,-0.5f},{0,0,1},{1,0,0}},
    {{ 0.5f, 0.5f, 0.5f},{1,1,1},{1,0,0}},
    // Left   (-X)
    {{-0.5f,-0.5f,-0.5f},{1,0,0},{-1,0,0}},
    {{-0.5f,-0.5f, 0.5f},{0,1,0},{-1,0,0}},
    {{-0.5f, 0.5f, 0.5f},{0,0,1},{-1,0,0}},
    {{-0.5f, 0.5f,-0.5f},{1,1,1},{-1,0,0}},
    // Top    (+Y)
    {{-0.5f, 0.5f, 0.5f},{1,0,0},{0,1,0}},
    {{ 0.5f, 0.5f, 0.5f},{0,1,0},{0,1,0}},
    {{ 0.5f, 0.5f,-0.5f},{0,0,1},{0,1,0}},
    {{-0.5f, 0.5f,-0.5f},{1,1,1},{0,1,0}},
    // Bottom (-Y)
    {{-0.5f,-0.5f,-0.5f},{1,0,0},{0,-1,0}},
    {{ 0.5f,-0.5f,-0.5f},{0,1,0},{0,-1,0}},
    {{ 0.5f,-0.5f, 0.5f},{0,0,1},{0,-1,0}},
    {{-0.5f,-0.5f, 0.5f},{1,1,1},{0,-1,0}},
  };

  const uint32_t indices[] = {
     0, 1, 2,  2, 3, 0,   // front
     4, 5, 6,  6, 7, 4,   // back
     8, 9,10, 10,11, 8,   // right
    12,13,14, 14,15,12,   // left
    16,17,18, 18,19,16,   // top
    20,21,22, 22,23,20,   // bottom
  };

  mesh->vertexCount = sizeof(vertices) / sizeof(vertices[0]);
  mesh->indexCount  = sizeof(indices) / sizeof(indices[0]);
  mesh->vertices = malloc(sizeof(Vertex) * mesh->vertexCount);
  mesh->indices  = malloc(sizeof(uint32_t) * mesh->indexCount);
  memcpy(mesh->vertices, vertices, sizeof(Vertex) * mesh->vertexCount);
  memcpy(mesh->indices, indices, sizeof(uint32_t) * mesh->indexCount);
  glm_mat4_identity(mesh->model);
}

void createCubeMeshNN(Mesh* mesh){
  const Vertex vertices[] = {
    // Front
    {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}},

    // Back
    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}
  };

  const uint16_t indices[] = {
    0,1,2, 2,3,0,
    5,4,7, 7,6,5,
    4,0,3, 3,7,4,
    1,5,6, 6,2,1,
    3,2,6, 6,7,3,
    4,5,1, 1,0,4
  };

  mesh->vertexCount = sizeof(vertices) / sizeof(vertices[0]);
  mesh->indexCount  = sizeof(indices) / sizeof(indices[0]);

  mesh->vertices = malloc(sizeof(Vertex) * mesh->vertexCount);
  mesh->indices  = malloc(sizeof(uint16_t) * mesh->indexCount);

  memcpy(mesh->vertices,
	 vertices,
	 sizeof(Vertex) * mesh->vertexCount);

  memcpy(mesh->indices,
	 indices,
	 sizeof(uint16_t) * mesh->indexCount);

  glm_mat4_identity(mesh->model);
}

void rotate_vectorX(vec3 a,float theta,vec3 d){
  d[0] = a[0];
  d[1] = cosf(theta)*a[1] + (-sinf(theta)*a[2]);
  d[2] = sinf(theta)*a[1] + cosf(theta)*a[2];
}


void createResultMz(World* world,Mesh* mesh,BeamElement* beam,Grid* grid){
  vec3 center1;
  Node node1 = world->nodes[beam->node1];
  center1[0] = node1.x;center1[1] = node1.y;center1[2] = node1.z;

  vec3 center2;
  Node node2 = world->nodes[beam->node2];
  center2[0] = node2.x;center2[1] = node2.y;center2[2] = node2.z;

  //NOTE: GET AXIS
  vec3 worldUP;
  worldUP[0] = 0.0;worldUP[1] = 0.0;worldUP[2] = 1.0;
  vec3 forward;
  glm_vec_sub(center2,center1,forward);
  glm_vec_normalize(forward);
  int num_divisions = 500;
  float step = beam->Le/num_divisions;

  int num_lines = num_divisions+1;

  grid->vertexCount = num_lines*2;
  grid->vertices = (LineVertex*)calloc(grid->vertexCount,sizeof(LineVertex));

  float E = world->materials[beam->mat_id].E;
  float Iz = world->sections[beam->sec_id].Iz;
  int idx = 0;
  for(int i=0;i<num_lines;i++){
    vec3 temp,current_center;
    float Delta = i*step;
    glm_vec_scale(forward,Delta,temp);
    glm_vec_add(center1,temp,current_center);
    float Mz = get_Mz(beam,(i*step)/beam->Le,E,Iz);
    Mz*=0.0005;
    grid->vertices[idx].position[0] = current_center[0];
    grid->vertices[idx].position[1] = current_center[1];
    grid->vertices[idx].position[2] = current_center[2];
    idx++;
    grid->vertices[idx].position[0] = current_center[0];
    grid->vertices[idx].position[1] = current_center[1];
    grid->vertices[idx].position[2] = current_center[2]+Mz;
    idx++;
  }
}

Vertex* convertOutlineVertices(World* world,BeamElement* beam,point2D* outline_points,int outline_count,float raduis,vec3 center1,vec3 up,vec3 right,float* stressValues,int stress_count,int* return_size);
