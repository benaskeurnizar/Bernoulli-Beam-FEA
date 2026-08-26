#include "section_outline.h"


float my_sigma_max = 2.5e6;
float my_sigma_min = -2.5e6;

void vec_negate(vec3 v){
  v[0]*= -1;
  v[1]*= -1;
  v[2]*= -1;
}

float clampf(float x, float min, float max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}
void stress_to_color(float t, float* r, float* g, float* b)
{
    t = clampf(t, 0.0f, 1.0f);

    if (t < 0.5f) {
        float s = t * 2.0f;       // 0 = blue, 1 = white
        *r = s;
        *g = s;
        *b = 1.0f;
    } else {
        float s = (t - 0.5f) * 2.0f; // 0 = white, 1 = red
        *r = 1.0f;
        *g = 1.0f - s;
        *b = 1.0f - s;
    }
}

void stress_to_color2(float t, float* r, float* g, float* b) {
    // 4 segments: blue->cyan->green->yellow->red
    if (t < 0.25f) {
        float s = t / 0.25f;
        *r = 0.0f;
        *g = s;
        *b = 1.0f;
    } else if (t < 0.5f) {
        float s = (t - 0.25f) / 0.25f;
        *r = 0.0f;
        *g = 1.0f;
        *b = 1.0f - s;
    } else if (t < 0.75f) {
        float s = (t - 0.5f) / 0.25f;
        *r = s;
        *g = 1.0f;
        *b = 0.0f;
    } else {
        float s = (t - 0.75f) / 0.25f;
        *r = 1.0f;
        *g = 1.0f - s;
        *b = 0.0f;
    }
}
void stress_to_color_sigma(float sigma, float* r, float* g, float* b)
{
    float t = (sigma - my_sigma_min) /
              (my_sigma_max - my_sigma_min);

    t = clampf(t, 0.0f, 1.0f);

    // blue -> cyan -> green -> yellow -> red

    if (t < 0.25f) {
        float s = t / 0.25f;

        *r = 0.0f;
        *g = s;
        *b = 1.0f;

    } else if (t < 0.5f) {
        float s = (t - 0.25f) / 0.25f;

        *r = 0.0f;
        *g = 1.0f;
        *b = 1.0f - s;

    } else if (t < 0.75f) {
        float s = (t - 0.5f) / 0.25f;

        *r = s;
        *g = 1.0f;
        *b = 0.0f;

    } else {
        float s = (t - 0.75f) / 0.25f;

        *r = 1.0f;
        *g = 1.0f - s;
        *b = 0.0f;
    }
}
point2D* createCircleOutline(float raduis,float deltaDeg,int* count){
  //const float DthetaDeg = 0.5f;
  int N = (int)(360.0f / deltaDeg + 0.5f); // round, don't truncate
  *count = N;
  point2D* points = (point2D*)calloc(*count,sizeof(point2D));

  for(int i=0;i<N;i++){
    float theta = glm_rad(i*deltaDeg);
    points[i].u = raduis*cosf(theta);
    points[i].v = raduis*sinf(theta);
  }
  return points;
}
point2D* createRectOutline(float width, float height, int segPerEdge, int* count) {
    float hw = 0.5f * width;
    float hh = 0.5f * height;
    int N = 4 * segPerEdge;
    *count = N;
    point2D* points = (point2D*)calloc(*count, sizeof(point2D));
    int idx = 0;

    float corners[4][2] = {
        {-hw, -hh}, { hw, -hh}, { hw,  hh}, {-hw,  hh}
    };

    for (int i = 0; i < 4; i++) {
        int i2 = (i + 1) % 4;
        float dx = corners[i2][0] - corners[i][0];
        float dy = corners[i2][1] - corners[i][1];
        float length = sqrtf(dx*dx + dy*dy);
        float nx = dx / length;
        float ny = dy / length;
        float step = length / (float)segPerEdge;

        for (int j = 0; j < segPerEdge; j++) {
            float add = j * step;
            points[idx].u = corners[i][0] + nx * add;
            points[idx].v = corners[i][1] + ny * add;
            idx++;
        }
    }
    return points;
}

float compute_sigmaX(World* world,BeamElement* beam,float y,float z,float position){
  Section section = world->sections[beam->sec_id];
  Material material = world->materials[beam->mat_id];
  int point_stress_count = 0;
  float* point_stresses =  getSectionStress(world,beam,position,&point_stress_count);
  float sigma = (point_stresses[0]/section.A)+(-point_stresses[5]*y/section.Iz)+(point_stresses[4]*z/section.Iy);
  
  return sigma;
}

float compute_shear(World* world,BeamElement* beam,float y,float z,float position){
  Section section = world->sections[beam->sec_id];
  Material material = world->materials[beam->mat_id];
  int point_stress_count = 0;
  float* point_stresses =  getSectionStress(world,beam,position,&point_stress_count);
  float side = sqrtf(section.A);
  float width  = side;
  float height = side;

  float Vy = point_stresses[1];
  float Vz = point_stresses[2];

  float Qz = (width/2)*((height*height)/4 - z*z);
  float Qy = (height/2)*((width*width)/4 - y*y );

  float tou_z = Vz * Qy / (section.Iy*height);
  float tou_y = Vy * Qz / (section.Iz*width);

  float result = sqrtf(tou_z*tou_z+tou_y*tou_y);
  return result;
}

float compute_von_mise(World* world,BeamElement* beam,float y,float z,float position){
  float sigma = compute_sigmaX(world,beam,y,z,position);
  float tou = compute_shear(world,beam,y,z,position);

  float result = sqrtf(sigma*sigma+3*tou*tou);
  return result;
}

Vertex* convertOutlineToVertices(World* world,BeamElement* beam,point2D* outline,int outline_count,vec3 center,vec3 forward,vec3 right,vec3 up,float pos,int* vertices_count,uint8_t stresses){
  Section section = world->sections[beam->sec_id];
  Material material = world->materials[beam->mat_id];
  Vertex* vertices = (Vertex*)calloc(outline_count,sizeof(Vertex));

  Vertex vert = {0};
  glm_vec_copy(forward,vert.normal);
  if(pos == 0.0){
    vec_negate(vert.normal);
  }
  int idx = 0;
  for(int i=0;i<outline_count;i++){
    float u = outline[i].u;
    float v = outline[i].v;
    vert.pos[0] = center[0]+u*right[0]+v*up[0];
    vert.pos[1] = center[1]+u*right[1]+v*up[1];
    vert.pos[2] = center[2]+u*right[2]+v*up[2];

    //NOTE: i assume that the u and v are the z and y???
    float sigma = 0.0;
    if(world->fea_solved){
      if(stresses & SIGMA_X){
	sigma = compute_sigmaX(world,beam,u,v,pos);
      }else if(stresses & TOU){
	sigma = compute_shear(world,beam,u,v,pos);
      }else if(stresses & VON_MISE){
	sigma = compute_von_mise(world,beam,u,v,pos);
      }
    }
    // normalize so that 0 stress = 0, max absolute stress = 1
    float abs_max = fmaxf(fabsf(my_sigma_max), fabsf(my_sigma_min));
    //float t = fabsf(sigma) / abs_max;
    //t = clampf(t, 0.0f, 1.0f);
    float t = (sigma - my_sigma_min) / (my_sigma_max - my_sigma_min);
    t = clampf(t, 0.0f, 1.0f);
    stress_to_color_sigma(sigma,&(vert.color[0]),&(vert.color[1]),&(vert.color[2]));
    memcpy(vertices+idx,&vert,sizeof(Vertex));
    idx++;
  }

  *vertices_count = idx;
  return vertices;
}

Vertex* meshInnerRings(World* world,BeamElement* beam,Vertex** rings,int ring_size,int ring_count,int* result_count,vec3 center,float position,uint8_t stresses){
  int count = ((ring_count-1)*(ring_size)*6)+(3*(ring_size));
  Vertex* result = (Vertex*)calloc(count,sizeof(Vertex));

  vec3 normal;//NOTE: I'LL JUST PICK A NORMAL FROM ONE OF THESE VERTICES...ASSUMING THEY ARE CORRECT

  int idx = 0;
  for(int i=0;i<ring_count-1;i++){
    int current_ring = i;
    int next_ring = (i+1);
    for(int j=0;j<ring_size;j++){
      if(i==0 && j==0){
	glm_vec_copy(rings[current_ring][j].normal,normal);
      }
      int j2 = (j+1)%ring_size;

      result[idx++] = rings[current_ring][j];
      result[idx++] = rings[current_ring][j2];
      result[idx++] = rings[next_ring][j];

      result[idx++] = rings[next_ring][j];
      result[idx++] = rings[next_ring][j2];
      result[idx++] = rings[current_ring][j2];
    }
  }
  Vertex center_vert = {0};
  glm_vec_copy(normal,center_vert.normal);
  Section section = world->sections[beam->sec_id];
  float sigma = 0.0;
  if(world->fea_solved){
    if(stresses & SIGMA_X){
      sigma = compute_sigmaX(world,beam,0.0,0.0,position);
    }else if(stresses & TOU){
      sigma = compute_shear(world,beam,0.0,0.0,position);
    }else if(stresses & VON_MISE){
      sigma = compute_von_mise(world,beam,0.0,0.0,position);
    }
    //float t = (sigma - my_sigma_min) / (my_sigma_max - my_sigma_min);
    //t = clampf(t, 0.0f, 1.0f);
    //stress_to_color(t,&(center_vert.color[0]),&(center_vert.color[1]),&(center_vert.color[2]));
    //memcpy(vertices+idx,&center_vert,sizeof(Vertex));
    //idx++;
  }
  float abs_max = fmaxf(fabsf(my_sigma_max), fabsf(my_sigma_min));
  //float t = fabsf(sigma) / abs_max;
  // t = clampf(t, 0.0f, 1.0f);
  float t = (sigma - my_sigma_min) / (my_sigma_max - my_sigma_min);
  t = clampf(t, 0.0f, 1.0f);
  stress_to_color_sigma(sigma,&(center_vert.color[0]),&(center_vert.color[1]),&(center_vert.color[2]));
  glm_vec_copy(center,center_vert.pos);
  for(int i=0;i<(ring_size);i++){
    int next_i = (i+1)%(ring_size);
    result[idx++] = center_vert;
    result[idx++] = rings[0][i];
    result[idx++] = rings[0][next_i];
  }
  *result_count = idx;
  return result;
}

Vertex* createMeshedSection(World* world,BeamElement* beam,vec3 center,vec3 forward,vec3 right,vec3 up,float position,uint8_t stresses,int num_divisions,int* vertex_count){
  Section section = world->sections[beam->sec_id];
  float A = section.A;

  point2D** outline_points = (point2D**)malloc(sizeof(point2D*)*num_divisions);
  int outline_count = 0;

  float section_scale = 1.0;

  switch(section.type){
  case CIRCLE:
    {
      float raduis = sqrtf(A/M_PI)*section_scale;;
      for(int i = 0;i<num_divisions;i++){
	float r = raduis/(float)(num_divisions-i);
	int count;
	outline_points[i] =  createCircleOutline(r,1.0,&count);
	if(count > outline_count){
	  outline_count = count;
	}
      }
    }break;
  case RECTANGLE:
    {
      float side = sqrtf(A);
      float width  = side*section_scale;
      float height = side*section_scale;
       for(int i = 0;i<num_divisions;i++){
	float w = width/(float)(num_divisions-i);
	float h = height/(float)(num_divisions-i);
	int count;
	outline_points[i] =  createRectOutline(w,h,10,&count);
	if(count > outline_count){
	  outline_count = count;
	}
      }
    }break;
  }

  Vertex** inner_rings = (Vertex**)malloc(sizeof(Vertex*)*num_divisions);
  for(int i=0;i<num_divisions;i++){
    int count;
    inner_rings[i] =  convertOutlineToVertices(world,beam,outline_points[i],outline_count,center,forward,right,up,position,&count,stresses);
  }

  //NOTE : NOW MESH THE INNER RINGS INTO ONE
  int result_count;
  Vertex* result = meshInnerRings(world,beam,inner_rings,outline_count,num_divisions,&result_count,center,position,stresses);
  *vertex_count = result_count;

  for(int i=0;i<num_divisions;i++){
    free(outline_points[i]);
    free(inner_rings[i]);
  }
  free(outline_points);
  free(inner_rings);

  return result;
}


Vertex* connectSectionsLong(Vertex** rings,int num_rings,int ring_size,int* vertex_count){
  int result_size = 6*ring_size*(num_rings-1);
  Vertex* result = (Vertex*)calloc(result_size,sizeof(Vertex));
  int idx = 0;
  for(int i=0;i<num_rings-1;i++){
    int current_ring = i;
    int next_ring = (i+1)%num_rings;

    for(int j=0;j<ring_size;j++){
      int j2 = (j+1)%ring_size;
      
      memcpy(result+idx,&(rings[current_ring][j]),sizeof(Vertex));
      idx++;
    
      memcpy(result+idx,&(rings[next_ring][j]),sizeof(Vertex));
      idx++;
  
      memcpy(result+idx,&(rings[current_ring][j2]),sizeof(Vertex));
      idx++;
    
      memcpy(result+idx,&(rings[current_ring][j2]),sizeof(Vertex));
      idx++;
    
      memcpy(result+idx,&(rings[next_ring][j]),sizeof(Vertex));
      idx++;
    
      memcpy(result+idx,&(rings[next_ring][j2]),sizeof(Vertex));
      idx++;
    }
  }
  if(idx!=result_size){
    printf("connectSectionsLong : vertices count mismatch!!\n");
  }
  *vertex_count = idx;
  return result;
}
