#include "draw_elements.h"
#include "section_outline.h"
#include "stdint.h"

void rotateAroundAxis(vec3 v, vec3 axis, float angle, vec3 result)
{
    float c = cosf(angle);
    float s = sinf(angle);

    vec3 cross;
    glm_vec_cross(axis, v, cross);

    float dot = glm_vec_dot(axis, v);

    result[0] = v[0] * c + cross[0] * s + axis[0] * dot * (1.0f - c);
    result[1] = v[1] * c + cross[1] * s + axis[1] * dot * (1.0f - c);
    result[2] = v[2] * c + cross[2] * s + axis[2] * dot * (1.0f - c);
}

void applyDeformations(World* world, BeamElement* beam, float position,
                       vec3 center, vec3 forward, vec3 up, vec3 right,
                       uint8_t displacements) {
    float scale  = 1000.0f;
    float scale2 = 1.0f;   // no exaggeration on rotations usually
    
    if(!world->fea_solved) return;

    float z_dis=0, y_dis=0, x_dis=0;
    float theta_x=0, theta_y=0, theta_z=0;

    if(displacements & DIS_Z)
        z_dis = get_z_displacement(beam,position,beam->local_dis[2],beam->local_dis[4],beam->local_dis[8],beam->local_dis[10]);
    if(displacements & DIS_Y)
        y_dis = get_y_displacement(beam,position,beam->local_dis[1],beam->local_dis[5],beam->local_dis[7],beam->local_dis[11]);
    if(displacements & DIS_X)
        x_dis = get_x_displacement(beam,position,beam->local_dis[0],beam->local_dis[6]);
    if(displacements & ROT_X)
        theta_x = get_x_rotation(beam,position,beam->local_dis[3],beam->local_dis[9]);
    if(displacements & ROT_Y)
        theta_y = get_y_rotation(beam,position,beam->local_dis[1],beam->local_dis[5],beam->local_dis[7],beam->local_dis[11]);
    if(displacements & ROT_Z)
        theta_z = get_z_rotation(beam,position,beam->local_dis[2],beam->local_dis[4],beam->local_dis[8],beam->local_dis[10]);

    // translate center
    vec3 temp;
    glm_vec_scale(up,      z_dis * scale, temp); glm_vec_add(center, temp, center);
    glm_vec_scale(right,   y_dis * scale, temp); glm_vec_add(center, temp, center);
    glm_vec_scale(forward, x_dis * scale, temp); glm_vec_add(center, temp, center);

    // rotate frame sequentially — order: twist, then bend Y, then bend Z
    rotateAroundAxis(right, forward, theta_x * scale2, right);
    rotateAroundAxis(up,    forward, theta_x * scale2, up);

    rotateAroundAxis(forward, right, theta_y * scale2, forward);
    rotateAroundAxis(up,      right, theta_y * scale2, up);

    rotateAroundAxis(forward, up, theta_z * scale2, forward);
    rotateAroundAxis(right,   up, theta_z * scale2, right);

    // orthonormalize
    glm_vec_normalize(forward);
    glm_vec_cross(forward, up, right);
    glm_vec_normalize(right);
    glm_vec_cross(right, forward, up);
    glm_vec_normalize(up);
}

void applyDeformations2(World* world,BeamElement* beam,float position,vec3 center,vec3 forward,vec3 up,vec3 right,uint8_t displacements){
  float scale = 1000.0;
  float scale2 = 10.0;

  float z_dis = 0.0,y_dis = 0.0,x_dis = 0.0;
  float theta_x = 0.0,theta_y = 0.0,theta_z = 0.0;
  if(!world->fea_solved){
    return;
  }
  if(displacements & DIS_Z){
    z_dis = get_z_displacement(beam,position,beam->local_dis[2],beam->local_dis[4],beam->local_dis[8],beam->local_dis[10]);
  }
  if(displacements & DIS_Y){
    y_dis = get_y_displacement(beam,position,beam->local_dis[1],beam->local_dis[5],beam->local_dis[7],beam->local_dis[11]);
  }
  if(displacements & DIS_X){
    x_dis = get_x_displacement(beam,position,beam->local_dis[0],beam->local_dis[6]);
  }
  if(displacements & ROT_X){
    theta_x = get_x_rotation(beam,position,beam->local_dis[3],beam->local_dis[9]);
  }
  if(displacements & ROT_Z){
    theta_z = get_z_rotation(beam,position,beam->local_dis[2],beam->local_dis[4],beam->local_dis[8],beam->local_dis[10]);
  }
  if(displacements & ROT_Y){
    theta_y = get_y_rotation(beam,position,beam->local_dis[1],beam->local_dis[5],beam->local_dis[7],beam->local_dis[11]);
  }

  vec3 temp1,temp2,temp3;
  glm_vec_scale(up,z_dis*scale,temp1);
  glm_vec_scale(right,y_dis*scale,temp2);
  glm_vec_scale(forward,x_dis*scale,temp3);
  glm_vec_add(center,temp1,center);
  glm_vec_add(center,temp2,center);
  glm_vec_add(center,temp3,center);

  // Save ORIGINAL orthogonal frame before any rotation
  vec3 orig_forward, orig_up, orig_right;
  glm_vec_copy(forward, orig_forward);
  glm_vec_copy(up, orig_up);
  glm_vec_copy(right, orig_right);

  // Apply each rotation to a FRESH copy of the original frame, then combine by summing the deltas
  vec3 f1,u1,r1;
  glm_vec_copy(orig_forward,f1); glm_vec_copy(orig_up,u1); glm_vec_copy(orig_right,r1);
  rotateAroundAxis(r1, orig_forward, theta_x*scale2, r1);
  rotateAroundAxis(u1, orig_forward, theta_x*scale2, u1);

  vec3 f2,u2,r2;
  glm_vec_copy(orig_forward,f2); glm_vec_copy(orig_up,u2); glm_vec_copy(orig_right,r2);
  rotateAroundAxis(f2, orig_right, theta_y*scale2, f2);
  rotateAroundAxis(u2, orig_right, theta_y*scale2, u2);

  vec3 f3,u3,r3;
  glm_vec_copy(orig_forward,f3); glm_vec_copy(orig_up,u3); glm_vec_copy(orig_right,r3);
  rotateAroundAxis(f3, orig_up, theta_z*scale2, f3);
  rotateAroundAxis(r3, orig_up, theta_z*scale2, r3);

  // Combine: sum the deviations from original, add back to original
  vec3 df1,df2,df3, dfsum;
  glm_vec_sub(f2, orig_forward, df1);
  glm_vec_sub(f3, orig_forward, df2);
  glm_vec_add(orig_forward, df1, forward);
  glm_vec_add(forward, df2, forward);

  vec3 du1,du2;
  glm_vec_sub(u1, orig_up, du1);
  glm_vec_sub(u2, orig_up, du2);
  glm_vec_add(orig_up, du1, up);
  glm_vec_add(up, du2, up);

  vec3 dr1,dr2;
  glm_vec_sub(r1, orig_right, dr1);
  glm_vec_sub(r3, orig_right, dr2);
  glm_vec_add(orig_right, dr1, right);
  glm_vec_add(right, dr2, right);

  glm_vec_normalize(forward);
  glm_vec_cross(forward, up, right);
  glm_vec_normalize(right);
  glm_vec_cross(right, forward, up);
  glm_vec_normalize(up);
}


void createElementVertices(World* world,BeamElement* beam,Mesh* mesh,uint8_t displacements,uint8_t stresses,int first){
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
  if (fabsf(glm_vec_dot(forward, worldUP)) > 0.99f){
    worldUP[1] = 1.0; // avoid parallel case
    worldUP[2] = 0.0;
  }
  vec3 right,up;
  glm_vec_cross(worldUP,forward,right);glm_vec_normalize(right);  // right = (0,1,0) ✓
  glm_vec_cross(forward,right,up);glm_vec_normalize(up);           // up = (1,0,0)×(0,1,0) = (0,0,1) ✓

  vec3 base_forward,base_right,base_up;
  glm_vec_copy(forward,base_forward);
  glm_vec_copy(up,base_up);
  glm_vec_copy(right,base_right);

  // deform the first and last centers
  vec3 local_fwd1, local_up1, local_right1;
  glm_vec_copy(base_forward, local_fwd1);
  glm_vec_copy(base_up, local_up1);
  glm_vec_copy(base_right, local_right1);
  vec3 center1_deformed;
  glm_vec_copy(center1,center1_deformed);
  applyDeformations(world, beam, 0.0,center1_deformed, local_fwd1, local_up1, local_right1,displacements);

  vec3 local_fwd2, local_up2, local_right2;
  glm_vec_copy(base_forward, local_fwd2);
  glm_vec_copy(base_up, local_up2);
  glm_vec_copy(base_right, local_right2);
  vec3 center2_deformed;
  glm_vec_copy(center2,center2_deformed);
  applyDeformations(world, beam, 1.0,center2_deformed, local_fwd2, local_up2, local_right2,displacements);

  //NOTE: create the 2 meshed rings for the outer faces
  int num_inner_divisions = 20;
  int sec1_size;
  Vertex* sec1_verts = createMeshedSection(world,beam,center1_deformed,local_fwd1,local_right1,local_up1,0.0,stresses,num_inner_divisions,&sec1_size);

  int sec2_size;
  Vertex* sec2_verts = createMeshedSection(world,beam,center2_deformed,local_fwd2,local_right2,local_up2,1.0,stresses,num_inner_divisions,&sec2_size);

  //NOTE: NOW THE LONG ONE
  int num_long_divisions = 20;
  float long_step = beam->Le/(float)num_long_divisions;
  int long_rings_count = num_long_divisions+1;
  Vertex** long_rings = (Vertex**)malloc(sizeof(Vertex*)*long_rings_count); 
  int ring_size = 0;
  vec3 prev_center;
  glm_vec_copy(center1,prev_center);
  for(int i=0;i<long_rings_count;i++){
    float xi = long_step*i/beam->Le;
    vec3 current_center, temp2;
    glm_vec_copy(prev_center, current_center);
    
    if (i > 0) {
      glm_vec_scale(base_forward, long_step, temp2);
      glm_vec_add(current_center, temp2, current_center);
    }
    
    glm_vec_copy(current_center, prev_center);
    
    // COPY FRESH BASE VECTORS EVERY ITERATION
    vec3 local_fwd, local_up, local_right;
    glm_vec_copy(base_forward, local_fwd);
    glm_vec_copy(base_up, local_up);
    glm_vec_copy(base_right, local_right);
    
    // Pass local copies, NOT the mutated outer vectors
    applyDeformations(world, beam, xi, current_center, local_fwd, local_up, local_right,displacements);
    int size;
    long_rings[i] = createMeshedSection(world,beam,current_center,local_fwd,local_right,local_up,xi,stresses,1,&size);
    ring_size = size;
    /*if(size > ring_size){
      ring_size = size;
      }*/
  }

  int long_verts_count;
  Vertex* long_verts = connectSectionsLong(long_rings,long_rings_count,ring_size,&long_verts_count);

  if(mesh->vertices){
    free(mesh->vertices);
  }
  if(mesh->indices){
    free(mesh->indices);
  }

  mesh->vertexCount = long_verts_count+sec1_size+sec2_size;
  mesh->vertices = (Vertex*)calloc(mesh->vertexCount,sizeof(Vertex));
  mesh->indexCount = mesh->vertexCount;
  mesh->indices = (uint32_t*)calloc(mesh->indexCount,sizeof(uint32_t));

  printf("sec1_size=%d sec2_size=%d long_verts_count=%d\n", sec1_size, sec2_size, long_verts_count);
  printf("mesh->vertexCount=%d\n", mesh->vertexCount);
 
  int idx = 0;
  memcpy(mesh->vertices+idx,sec1_verts,sec1_size*sizeof(Vertex));
  for(int i=0;i<sec1_size;i++){
    mesh->indices[i] = i;
  }
  idx+=sec1_size;

  memcpy(mesh->vertices+idx,sec2_verts,sec2_size*sizeof(Vertex));
  for(int i=idx;i<idx+sec2_size;i++){
    mesh->indices[i] = i;
  }
  idx+=sec2_size;

  memcpy(mesh->vertices+idx,long_verts,long_verts_count*sizeof(Vertex));
  for(int i=idx;i<idx+long_verts_count;i++){
    mesh->indices[i] = i;
  }
  idx+=long_verts_count;
  printf("final idx=%d\n", idx);
  for(int i=0;i<long_rings_count;i++){
    free(long_rings[i]);
  }
  free(long_rings);
  free(sec1_verts);
  free(sec2_verts);
  free(long_verts);

  if(!first){
    world->update_beam_buffers = 1;
  }

  glm_mat4_identity(mesh->model);
  if(world->fea_solved){
    Section section = world->sections[beam->sec_id];
    float A = section.A;
    float side = sqrtf(A);
    float width  = side;
    float height = side;
   
    printf("\n \n");
    printf("Element 1 section 0 stresses : \n");
    float z_start = height/2;
    float y = 0;
    int num_div = 20;
    float step = height/num_div;
    for(int i=0;i<num_div+1;i++){
      float z = z_start - (i*step);
      float sigma = compute_sigmaX(world,beam,y,z,0.0);
      printf("z = %.4f ; SIGMA = %.4f \n",z,sigma);
    }

    printf("\n \n");
    printf("Element 1 section 1 stresses : \n");
    z_start = height/2;
    y = 0;
    for(int i=0;i<num_div+1;i++){
      float z = z_start - (i*step);
      float sigma = compute_sigmaX(world,beam,y,z,1.0);
      printf("z = %.4f ; SIGMA = %.4f \n",z,sigma);
    }
  }
}
