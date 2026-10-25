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


Vertex* convertOutlineVertices(World* world,BeamElement* beam,point2D* outline_points,int outline_count,float raduis,vec3 center1,vec3 up,vec3 right,float* stressValues,int stress_count,int* return_size);


float rayInspection(World* world,int mouseX,int mouseY,vec3 node1,vec3 node2,float* dst){
  float ndcx = ((float)mouseX/world->width)*2.0 - 1.0;
  //float ndcy = 1.0 - ((float)mouseY/world->height)*2.0;
  float ndcy = ((float)mouseY/world->height)*2.0 - 1.0;
  vec4 ray_origin = {ndcx,ndcy,0.0,1.0};
  vec4 ray_far = {ndcx,ndcy,1.0,1.0};
  vec4 pa,pb;
  mat4 proj_view,inv_proj_view;
  glm_mat4_mul(world->proj,world->camera.view,proj_view);
  glm_mat4_inv(proj_view,inv_proj_view);
  
  glm_mat4_mulv(inv_proj_view,ray_origin,pa);glm_vec_scale(pa,1/pa[3],pa);
  glm_mat4_mulv(inv_proj_view,ray_far,pb);glm_vec_scale(pb,1/pb[3],pb);

  vec3 r0 = {pa[0], pa[1], pa[2]};
  vec3 rf = {pb[0], pb[1], pb[2]};
  
  vec3 ray_dir;
  glm_vec_sub(rf,r0,ray_dir);
  glm_vec_normalize(ray_dir);

  vec3 sd;
  glm_vec_sub(node2,node1,sd);
  vec3 sdn;
  glm_vec_copy(sd,sdn);
  glm_vec_normalize(sdn);
  
  vec3 RdN1;
  glm_vec_sub(r0,node1,RdN1);

  float a1 = ray_dir[0]*ray_dir[0] + ray_dir[1]*ray_dir[1] + ray_dir[2]*ray_dir[2];
  float b1 = -glm_vec_dot(ray_dir,sd);
  float c1 = glm_vec_dot(ray_dir,RdN1);

  float a2 = -b1;
  float b2 = -(sd[0]*sd[0] + sd[1]*sd[1] + sd[2]*sd[2]);
  float c2 = glm_vec_dot(sd,RdN1);

  float denom = (b2 - a2*b1/a1);
  if(fabsf(denom) < 1e-6f || fabsf(a1) < 1e-6f){
    *dst = 1e9;
    return 0.0;
  }
  
  float s = ((a2*c1/a1 - c2))/(b2 - a2*b1/a1);
  if(s>1.0){
    s = 1.0;
  }
  if(s<0.0){
    s = 0.0;
  }
  float t = -(b1*s + c1)/a1;

  t = fmaxf(t, 0.0f);
  
  vec3 P,S,temp1,temp2;
  glm_vec_scale(ray_dir,t,temp1);
  glm_vec_add(r0,temp1,P);

  glm_vec_scale(sd,s,temp2);
  glm_vec_add(node1,temp2,S);
  
  vec3 distance;
  glm_vec_sub(P,S,distance);
  *dst = sqrtf(distance[0]*distance[0] + distance[1]*distance[1] + distance[2]*distance[2]);

  return s;
}
