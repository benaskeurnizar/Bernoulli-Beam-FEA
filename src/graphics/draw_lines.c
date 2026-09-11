#include "draw_lines.h"
#include "fea_globals.h"

void createElementLines(FeaModel* model,Renderer* renderer,BeamElement* beam,int element_index,Diagram* diagram,DIAGRAM_TYPE type,int num_divisions);

void createBeamDiagram(FeaModel* model,Renderer* renderer,Diagram* diagram,DIAGRAM_TYPE type){
  int num_divisions = 50;
  diagram->vertexCount = (num_divisions+1)*model->num_beam_elements*2;
  diagram->vertices = (LineVertex*)calloc(diagram->vertexCount,sizeof(LineVertex));
  for(int i=0;i<model->num_beam_elements;i++){
    createElementLines(model,renderer,&(model->beams[i]),i,diagram,type,num_divisions);
  }

   
}

float getDiagramScale(DIAGRAM_TYPE type, float targetVisualHeight) {
  float max_val = 0.0f;
  float min_val = 0.0f;

  switch (type) {
  case DIAGRAM_N:  max_val = MAX_AXIAL_FORCE; min_val = MIN_AXIAL_FORCE; break;
  case DIAGRAM_VY: max_val = MAX_SHEAR_VY;    min_val = MIN_SHEAR_VY;    break;
  case DIAGRAM_VZ: max_val = MAX_SHEAR_VZ;    min_val = MIN_SHEAR_VZ;    break;
  case DIAGRAM_T:  max_val = MAX_TORQUE_MX;   min_val = MIN_TORQUE_MX;   break;
  case DIAGRAM_MY: max_val = MAX_MOMENT_MY;   min_val = MIN_MOMENT_MY;   break;
  case DIAGRAM_MZ: max_val = MAX_MOMENT_MZ;   min_val = MIN_MOMENT_MZ;   break;
  }

  // Determine the absolute maximum bound across positive and negative peaks
  float abs_max = fmaxf(fabsf(max_val), fabsf(min_val));

  // Guard against division by zero when there are no forces/moments present
  if (abs_max < 1e-6f) {
    return 0.0f;
  }

  // Scale factor to normalize maximum diagram ordinate to targetVisualHeight (in world units)
  return targetVisualHeight / abs_max;
}

void createElementLines(FeaModel* model,Renderer* renderer,BeamElement* beam,int element_index,Diagram* diagram,DIAGRAM_TYPE type,int num_divisions){
  Section section = model->sections[beam->sec_id];
  Material material = model->materials[beam->mat_id];
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
  float step = beam->Le/(float)num_divisions;

  vec3 prev_center;
  glm_vec_copy(center1,prev_center);
  int idx = 0;
  float TARGET_VISUAL_HEIGHT = 0.5f; 
  float value_scale = getDiagramScale(type, TARGET_VISUAL_HEIGHT);
  for(int i=0;i<num_divisions+1;i++){
    float position = step*i/beam->Le;

    vec3 current_center, temp2;
    glm_vec_copy(prev_center, current_center);
    
    if (i > 0) {
      glm_vec_scale(forward, step, temp2);
      glm_vec_add(current_center, temp2, current_center);
    }
    
    glm_vec_copy(current_center, prev_center);
    
    float value =calculateDiagramValue(model,beam,position,type);
    
    LineVertex pt1,pt2;
    pt1.position[0] = current_center[0];pt1.position[1] = current_center[1];pt1.position[2] = current_center[2];
    vec3 offset, temp3;
    glm_vec_scale(up, value * value_scale, offset);
    glm_vec_add(current_center, offset, temp3);

    pt2.position[0] = temp3[0];
    pt2.position[1] = temp3[1];
    pt2.position[2] = temp3[2];
    diagram->vertices[(element_index*(num_divisions+1)*2)+idx++] = pt1;
    diagram->vertices[(element_index*(num_divisions+1)*2)+idx++] = pt2;
  }
}


