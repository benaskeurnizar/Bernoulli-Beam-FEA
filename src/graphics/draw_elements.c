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

// Calculates scale factor to keep maximum displacement visually bounded
float getDeformationScale(float targetMaxVisualOffset, float max_x, float min_x, float max_y, float min_y, float max_z, float min_z) {
  // Find the global maximum absolute displacement across all 3 axes
  float max_disp = fmaxf(fabsf(max_x), fabsf(min_x));
  max_disp = fmaxf(max_disp, fmaxf(fabsf(max_y), fabsf(min_y)));
  max_disp = fmaxf(max_disp, fmaxf(fabsf(max_z), fabsf(min_z)));

  if (max_disp < 1e-7f) return 0.0f; // Guard against zero displacement

  // Scale factor so peak deformation reaches target visual offset (e.g., 0.2 world units)
  return targetMaxVisualOffset / max_disp;
}

// Calculates scale factor for rotations (in radians)
float getRotationScale(float targetMaxAngleRad, float max_rx, float min_rx, float max_ry, float min_ry, float max_rz, float min_rz) {
  float max_rot = fmaxf(fabsf(max_rx), fabsf(min_rx));
  max_rot = fmaxf(max_rot, fmaxf(fabsf(max_ry), fabsf(min_ry)));
  max_rot = fmaxf(max_rot, fmaxf(fabsf(max_rz), fabsf(min_rz)));

  if (max_rot < 1e-7f) return 0.0f;

  // Scale factor so peak rotation maps to targetMaxAngleRad (e.g., 15 degrees in rad)
  return targetMaxAngleRad / max_rot;
}

void applyDeformations(FeaModel* model, BeamElement* beam, float position,
                       vec3 center, vec3 forward, vec3 up, vec3 right,
                       uint32_t render_flags) {
  float TARGET_MAX_DISPLACEMENT = 0.2f;            // Peak displacement will move center by 0.2 units
  float TARGET_MAX_ROTATION = glm_rad(15.0f);  // Peak rotation will tilt frame by 15 degrees

  float scale = getDeformationScale(
				    TARGET_MAX_DISPLACEMENT, 
				    MAX_DEFORM_X, MIN_DEFORM_X, 
				    MAX_DEFORM_Y, MIN_DEFORM_Y, 
				    MAX_DEFORM_Z, MIN_DEFORM_Z
				    );

  float scale2 = getRotationScale(
				  TARGET_MAX_ROTATION, 
				  MAX_ROT_X, MIN_ROT_X, 
				  MAX_ROT_Y, MIN_ROT_Y, 
				  MAX_ROT_Z, MIN_ROT_Z
				  );
    
    if(!model->fea_solved) return;

    float z_dis=0, y_dis=0, x_dis=0;
    float theta_x=0, theta_y=0, theta_z=0;

    if(render_flags & SHOW_DEFORM_Z){
      z_dis = get_z_displacement(beam,position,beam->local_dis[2],beam->local_dis[4],beam->local_dis[8],beam->local_dis[10]);
    }
    if(render_flags & SHOW_DEFORM_Y){
      y_dis = get_y_displacement(beam,position,beam->local_dis[1],beam->local_dis[5],beam->local_dis[7],beam->local_dis[11]);
    }
    if(render_flags & SHOW_DEFORM_X){
      x_dis = get_x_displacement(beam,position,beam->local_dis[0],beam->local_dis[6]);
    }
    if(render_flags & SHOW_ROT_X){
      theta_x = get_x_rotation(beam,position,beam->local_dis[3],beam->local_dis[9]);
    }
    if(render_flags & SHOW_ROT_Z){
      theta_z = get_z_rotation(beam,position,beam->local_dis[2],beam->local_dis[4],beam->local_dis[8],beam->local_dis[10]);
    }
    if(render_flags & SHOW_ROT_Y){
      theta_y = get_y_rotation(beam,position,beam->local_dis[1],beam->local_dis[5],beam->local_dis[7],beam->local_dis[11]);
    }
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


void createElementVertices(FeaModel* model,BeamElement* beam,Mesh* mesh,uint32_t render_flags){
  vec3 center1;
  Node node1 = model->nodes[beam->node1];
  center1[0] = node1.x;center1[1] = node1.y;center1[2] = node1.z;

  vec3 center2;
  Node node2 = model->nodes[beam->node2];
  center2[0] = node2.x;center2[1] = node2.y;center2[2] = node2.z;

  //NOTE: GET AXIS
  vec3 modelUP;
  modelUP[0] = 0.0;modelUP[1] = 0.0;modelUP[2] = 1.0;
  vec3 forward;
  glm_vec_sub(center2,center1,forward);
  glm_vec_normalize(forward);
  if (fabsf(glm_vec_dot(forward, modelUP)) > 0.99f){
    modelUP[1] = 1.0; // avoid parallel case
    modelUP[2] = 0.0;
  }
  vec3 right,up;
  glm_vec_cross(modelUP,forward,right);glm_vec_normalize(right);  // right = (0,1,0) 
  glm_vec_cross(forward,right,up);glm_vec_normalize(up);           // up = (1,0,0)×(0,1,0) = (0,0,1) 

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
  applyDeformations(model, beam, 0.0,center1_deformed, local_fwd1, local_up1, local_right1,render_flags);

  vec3 local_fwd2, local_up2, local_right2;
  glm_vec_copy(base_forward, local_fwd2);
  glm_vec_copy(base_up, local_up2);
  glm_vec_copy(base_right, local_right2);
  vec3 center2_deformed;
  glm_vec_copy(center2,center2_deformed);
  applyDeformations(model, beam, 1.0,center2_deformed, local_fwd2, local_up2, local_right2,render_flags);

  //NOTE: create the 2 meshed rings for the outer faces
  int num_inner_divisions = 20;
  int sec1_size;
  Vertex* sec1_verts = createMeshedSection(model,beam,center1_deformed,local_fwd1,local_right1,local_up1,0.0,render_flags,num_inner_divisions,&sec1_size);

  int sec2_size;
  Vertex* sec2_verts = createMeshedSection(model,beam,center2_deformed,local_fwd2,local_right2,local_up2,1.0,render_flags,num_inner_divisions,&sec2_size);

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
    applyDeformations(model, beam, xi, current_center, local_fwd, local_up, local_right,render_flags);
    int size;
    long_rings[i] = createMeshedSection(model,beam,current_center,local_fwd,local_right,local_up,xi,render_flags,1,&size);
    ring_size = size;
    /*if(size > ring_size){
      ring_size = size;
      }*/
  }

  int long_verts_count;
  Vertex* long_verts = connectSectionsLong(long_rings,long_rings_count,ring_size,&long_verts_count);

  int first = 0;

  if(mesh->vertices){
    free(mesh->vertices);
    
  }else{
    first = 1;
  }
  if(mesh->indices){
    free(mesh->indices);
  }else{
    first = 1;
  }

  if(!first){
    mesh->updated = 1;
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


  glm_mat4_identity(mesh->model);
  if(model->fea_solved){
    Section section = model->sections[beam->sec_id];
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
      float sigma = compute_sigmaX(model,beam,y,z,0.0);
      printf("z = %.4f ; SIGMA = %.4f \n",z,sigma);
    }

    printf("\n \n");
    printf("Element 1 section 1 stresses : \n");
    z_start = height/2;
    y = 0;
    for(int i=0;i<num_div+1;i++){
      float z = z_start - (i*step);
      float sigma = compute_sigmaX(model,beam,y,z,1.0);
      printf("z = %.4f ; SIGMA = %.4f \n",z,sigma);
    }
  }
}
