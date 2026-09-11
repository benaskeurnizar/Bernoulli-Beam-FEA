#include "fea_types.h"
#include "form_functions.h"
#include "matrices.h"
#include "beam_result.h"


float my_fpow(float val,int p){
  float result = 1.0;
  for(int i=0;i<p;i++){
    result *= val;
  }
  return result;
}

float get_z_displacement(BeamElement* bar,float position,float v1,float theta_z1,float v2,float theta_z2){
  float local_pos = position;
  //TODO: hardcoded
  //int N_indexes[4] = {1,5,7,11};
  int N_indexes[4] = {2,4,8,10};   // was {1,5,7,11}
  float values[4] = {v1,theta_z1,v2,theta_z2};
  int row_index = 2;
  float result = 0.0;
  for(int i=0;i<4;i++){
    int index = N_indexes[i];
    for(int j=0;j<bar->N[row_index][index].size;j++){
      result += (values[i]*bar->N[row_index][index].n[j]*my_fpow(local_pos,j)); 
    }
  }
  return result;
}

float get_y_displacement(BeamElement* bar,float position,float w1,float theta_y1,float w2,float theta_y2){
  float local_pos = position;
  int N_indexes[4] = {1,5,7,11};   // was {1,5,7,11}
  float values[4] = {w1,theta_y1,w2,theta_y2};
  int row_index = 1;
  float result = 0.0;
  for(int i=0;i<4;i++){
    int index = N_indexes[i];
    for(int j=0;j<bar->N[row_index][index].size;j++){
      result += (values[i]*bar->N[row_index][index].n[j]*my_fpow(local_pos,j)); 
    }
  }
  return result;
}

float get_x_displacement(BeamElement* bar,float position,float u1,float u2){
  float local_pos = position;
  int N_indexes[2] = {0,6};   // was {1,5,7,11}
  float values[2] = {u1,u2};
  int row_index = 0;
  float result = 0.0;
  for(int i=0;i<2;i++){
    int index = N_indexes[i];
    for(int j=0;j<bar->N[row_index][index].size;j++){
      result += (values[i]*bar->N[row_index][index].n[j]*my_fpow(local_pos,j)); 
    }
  }
  return result;
}

float get_x_rotation(BeamElement* bar,float position,float theta_x1,float theta_x2){
  float local_pos = position;
  int N_indexes[2] = {3,9};   // was {1,5,7,11}
  float values[2] = {theta_x1,theta_x2};
  int row_index = 3;
  float result = 0.0;
  for(int i=0;i<2;i++){
    int index = N_indexes[i];
    for(int j=0;j<bar->N[row_index][index].size;j++){
      result += (values[i]*bar->N[row_index][index].n[j]*my_fpow(local_pos,j)); 
    }
  }
  return result;
}

float get_z_rotation(BeamElement* bar,float position,float v1,float theta_z1,float v2,float theta_z2){
  float local_pos = position;
  float Linv = 1/bar->Le;
  //TODO: hardcoded
  //int N_indexes[4] = {1,5,7,11};
  int N_indexes[4] = {2,4,8,10};   // was {1,5,7,11}
  float values[4] = {v1,theta_z1,v2,theta_z2};
  int row_index = 2;
  float result = 0.0;
  for(int i=0;i<4;i++){
    int index = N_indexes[i];
    FormFunc N = bar->N[row_index][index];
    FormFunc* Nx =  deriveX(N);
    for(int j=0;j<Nx->size;j++){
      result += (values[i]*Nx->n[j]*my_fpow(local_pos,j)); 
    }
    free(Nx);
  }
  
  return result*Linv;
}


float get_y_rotation(BeamElement* bar,float position,float w1,float theta_y1,float w2,float theta_y2){
  float local_pos = position;
  float Linv = 1/bar->Le;
  int N_indexes[4] = {1,5,7,11};   // was {1,5,7,11}
  float values[4] = {w1,theta_y1,w2,theta_y2};
  int row_index = 1;
  float result = 0.0;
  for(int i=0;i<4;i++){
    int index = N_indexes[i];
    FormFunc N = bar->N[row_index][index];
    FormFunc* Nx =  deriveX(N);
    for(int j=0;j<Nx->size;j++){
      result += (values[i]*Nx->n[j]*my_fpow(local_pos,j)); 
    }
    free(Nx);
  }
  return -result*Linv;
}

float get_My(BeamElement* bar,float position,float E,float Iy){
  float EI = E * Iy ;

  
  FormFunc* N1cx = deriveX(bar->N[2][2]);
  scaleFormFunc(N1cx,1.0/bar->Le);
  FormFunc* N2cx = deriveX(bar->N[2][4]);
  scaleFormFunc(N2cx,1.0/bar->Le);
  FormFunc* N3cx = deriveX(bar->N[2][8]);
  scaleFormFunc(N3cx,1.0/bar->Le);
  FormFunc* N4cx = deriveX(bar->N[2][10]);
  scaleFormFunc(N4cx,1.0/bar->Le);

  FormFunc* N1cxx = deriveX(*N1cx);
  scaleFormFunc(N1cxx,1.0/bar->Le);
  FormFunc* N2cxx = deriveX(*N2cx);
  scaleFormFunc(N2cxx,1.0/bar->Le);
  FormFunc* N3cxx = deriveX(*N3cx);
  scaleFormFunc(N3cxx,1.0/bar->Le);
  FormFunc* N4cxx = deriveX(*N4cx);
  scaleFormFunc(N4cxx,1.0/bar->Le);

  float N1 = getFormValue(*N1cxx,position);
  float N2 = getFormValue(*N2cxx,position);
  float N3 = getFormValue(*N3cxx,position);
  float N4 = getFormValue(*N4cxx,position);

  

  float result =  (-N1*bar->local_dis[2])+(N2*bar->local_dis[4]) + (-N3*bar->local_dis[8]) + (N4*bar->local_dis[10]);
  //float result = (N1*bar->local_dis[2]) + (N2*bar->local_dis[4]) + (N3*bar->local_dis[8]) + (N4*bar->local_dis[10]);
  free(N1cx);free(N2cx);free(N3cx);free(N4cx);
  free(N1cxx);free(N2cxx);free(N3cxx);free(N4cxx);

  return result*EI;
}

float get_Mz(BeamElement* bar,float position,float E,float Iz){
  float EI = E * Iz ;
  
  FormFunc* N1cx = deriveX(bar->N[1][1]);
  scaleFormFunc(N1cx,1.0/bar->Le);
  FormFunc* N2cx = deriveX(bar->N[1][5]);
  scaleFormFunc(N2cx,1.0/bar->Le);
  FormFunc* N3cx = deriveX(bar->N[1][7]);
  scaleFormFunc(N3cx,1.0/bar->Le);
  FormFunc* N4cx = deriveX(bar->N[1][11]);
  scaleFormFunc(N4cx,1.0/bar->Le);

  FormFunc* N1cxx = deriveX(*N1cx);
  scaleFormFunc(N1cxx,1.0/bar->Le);
  FormFunc* N2cxx = deriveX(*N2cx);
  scaleFormFunc(N2cxx,1.0/bar->Le);
  FormFunc* N3cxx = deriveX(*N3cx);
  scaleFormFunc(N3cxx,1.0/bar->Le);
  FormFunc* N4cxx = deriveX(*N4cx);
  scaleFormFunc(N4cxx,1.0/bar->Le);

  float N1 = getFormValue(*N1cxx,position);
  float N2 = getFormValue(*N2cxx,position);
  float N3 = getFormValue(*N3cxx,position);
  float N4 = getFormValue(*N4cxx,position);

 
  float result =  (-N1*bar->local_dis[1]) + (-N2*bar->local_dis[5]) + (-N3*bar->local_dis[7]) + (-N4*bar->local_dis[11]);
  //float result = (N1*bar->local_dis[1]) + (N2*bar->local_dis[5]) + (N3*bar->local_dis[7]) + (N4*bar->local_dis[11]);
  free(N1cx);free(N2cx);free(N3cx);free(N4cx);
  free(N1cxx);free(N2cxx);free(N3cxx);free(N4cxx);

  return result*EI;
}

float integrate_dis_load(Load* force,float bound){
  FormFunc w;
  w.n = (float*)calloc(3,sizeof(float));
  w.size = 3;
  w.n[0] = force->x;
  w.n[1] = force->y;
  w.n[2] = force->z;

  return integrateX(w,0.0,bound);
}

float calculate_Vy_integral(FeaModel* model,BeamElement* beam,float bound){
  float result = 0.0;
  for(int i=0;i<model->num_forces;i++){
    if(model->forces[i].type == FORCE_DIS_Y && model->forces[i].element_id == beam->element_id){
      result += integrate_dis_load(&(model->forces[i]),bound)*beam->Le;
    }
  }
  return result;
}

float calculate_Vz_integral(FeaModel* model,BeamElement* beam,float bound){
  float result = 0.0;
  for(int i=0;i<model->num_forces;i++){
    if(model->forces[i].type == FORCE_DIS_Z && model->forces[i].element_id == beam->element_id){
      result += integrate_dis_load(&(model->forces[i]),bound)*beam->Le;
    }
  }
  return result;
}

float* getSectionStress(FeaModel* model,BeamElement* beam,float position,int* size){
  //NOTE: Calculate the beam's generalized strains at x
  int e_count = 4;
  float* E = (float*)calloc(e_count,sizeof(float));
  for(int i=0;i<beam->Nh;i++){
    for(int j=0;j<beam->Nw;j++){
      float value =  getFormValue(beam->B[i][j],position);
      E[i]+= (value*beam->local_dis[j]);
    }
  }

  //NOTE: Convert those into section forces
  int s_count = 4;
  float* S =  mulMatVec(beam->D,beam->dw,beam->dh,E,e_count,&s_count);

  Section section = model->sections[beam->sec_id];
  Material material = model->materials[beam->mat_id];
  float N = S[0];
  float T = S[1];
  float My = S[2];
  float Mz = S[3];
  float Vy = beam->local_forces[1]-calculate_Vy_integral(model,beam,position);
  float Vz = beam->local_forces[2]-calculate_Vz_integral(model,beam,position);
  *size = 6;
  float* result = (float*)malloc(sizeof(float)*(*size));
  float My2 = get_My(beam, position, material.E, section.Iy);
  float Mz2 = get_Mz(beam, position, material.E, section.Iz);
  result[0] = N;
  result[1] = Vy;
  result[2] = Vz;
  result[3] = T;
  result[4] = My;
  result[5] = Mz;
  return result;
}


float calculateDiagramValue(FeaModel* model,BeamElement* beam,float position,DIAGRAM_TYPE type){
  int e_count = 4;
  float* E = (float*)calloc(e_count,sizeof(float));
  for(int i=0;i<beam->Nh;i++){
    for(int j=0;j<beam->Nw;j++){
      float value =  getFormValue(beam->B[i][j],position);
      E[i]+= (value*beam->local_dis[j]);
    }
  }

  //NOTE: Convert those into section forces
  int s_count = 4;
  float* S =  mulMatVec(beam->D,beam->dw,beam->dh,E,e_count,&s_count);
  Section section = model->sections[beam->sec_id];
  Material material = model->materials[beam->mat_id];
  float value;
  switch(type){
  case DIAGRAM_MY:
    {
      value =get_My(beam,position,material.E,section.Iy) ;
    }break;
  case DIAGRAM_MZ:
    {
      value =get_Mz(beam,position,material.E,section.Iy) ;
    }break;
  case DIAGRAM_N:
    {
      value = S[0];
    }break;
  case DIAGRAM_T:
    {
      value = S[1];
    }break;
  case DIAGRAM_VY:
    {
      value = beam->local_forces[1]-calculate_Vy_integral(model,beam,position);;
    }break;
  case DIAGRAM_VZ:
    {
      value = beam->local_forces[2]-calculate_Vz_integral(model,beam,position);;
    }break;
  }
  free(E);
  free(S);
  return value;

}
