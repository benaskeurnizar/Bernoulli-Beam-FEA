#include "fea_globals.h"
#include "beam_result.h"
#include "graphics.h"


// Deformations
float MAX_DEFORM_X = 0.0f; float MIN_DEFORM_X = 0.0f;
float MAX_DEFORM_Y = 0.0f; float MIN_DEFORM_Y = 0.0f;
float MAX_DEFORM_Z = 0.0f; float MIN_DEFORM_Z = 0.0f;

// Rotations
float MAX_ROT_X = 0.0f; float MIN_ROT_X = 0.0f;
float MAX_ROT_Y = 0.0f; float MIN_ROT_Y = 0.0f;
float MAX_ROT_Z = 0.0f; float MIN_ROT_Z = 0.0f;

// Stresses
float MAX_AXIAL_STRESS = 0.0f; float MIN_AXIAL_STRESS = 0.0f;
float MAX_BEND_STRESS_Y = 0.0f; float MIN_BEND_STRESS_Y = 0.0f;
float MAX_BEND_STRESS_Z = 0.0f; float MIN_BEND_STRESS_Z = 0.0f;
float MAX_SHEAR_STRESS = 0.0f; float MIN_SHEAR_STRESS = 0.0f;
float MAX_VON_MISES = 0.0f; float MIN_VON_MISES = 0.0f;

// Diagrams
float MAX_AXIAL_FORCE = 0.0f; float MIN_AXIAL_FORCE = 0.0f;
float MAX_SHEAR_VY = 0.0f;    float MIN_SHEAR_VY = 0.0f;
float MAX_SHEAR_VZ = 0.0f;    float MIN_SHEAR_VZ = 0.0f;
float MAX_MOMENT_MY = 0.0f;   float MIN_MOMENT_MY = 0.0f;
float MAX_MOMENT_MZ = 0.0f;   float MIN_MOMENT_MZ = 0.0f;
float MAX_TORQUE_MX = 0.0f;   float MIN_TORQUE_MX = 0.0f;


void setDisGlobals(FeaModel* model,BeamElement* beam,int num_divisions){
  float step = beam->Le/num_divisions;
  for(int i=0;i<num_divisions+1;i++){
    float position = i*step/beam->Le;
    
    float z_dis = get_z_displacement(beam,position,beam->local_dis[2],beam->local_dis[4],beam->local_dis[8],beam->local_dis[10]);
    float y_dis = get_y_displacement(beam,position,beam->local_dis[1],beam->local_dis[5],beam->local_dis[7],beam->local_dis[11]);
    float x_dis = get_x_displacement(beam,position,beam->local_dis[0],beam->local_dis[6]);
    float theta_x = get_x_rotation(beam,position,beam->local_dis[3],beam->local_dis[9]);
    float theta_z = get_z_rotation(beam,position,beam->local_dis[2],beam->local_dis[4],beam->local_dis[8],beam->local_dis[10]);
    float theta_y = get_y_rotation(beam,position,beam->local_dis[1],beam->local_dis[5],beam->local_dis[7],beam->local_dis[11]);

    if(x_dis > MAX_DEFORM_X) MAX_DEFORM_X = x_dis;
    if(x_dis < MIN_DEFORM_X) MIN_DEFORM_X = x_dis;

    if(z_dis > MAX_DEFORM_Z) MAX_DEFORM_Z = z_dis;
    if(z_dis < MIN_DEFORM_Z) MIN_DEFORM_Z = z_dis;

    if(y_dis > MAX_DEFORM_Y) MAX_DEFORM_Y = y_dis;
    if(y_dis < MIN_DEFORM_Y) MIN_DEFORM_Y = y_dis;

    if(theta_x > MAX_ROT_X) MAX_ROT_X = theta_x;
    if(theta_x < MIN_ROT_X) MIN_ROT_X = theta_x;

    if(theta_y > MAX_ROT_Y) MAX_ROT_Y = theta_y;
    if(theta_y < MIN_ROT_Y) MIN_ROT_Y = theta_y;

    if(theta_z > MAX_ROT_Z) MAX_ROT_Z = theta_z;
    if(theta_z < MIN_ROT_Z) MIN_ROT_Z = theta_z;
  }
}

void setStressGlobals(FeaModel* model,BeamElement* beam,int num_divisions){
  float step = beam->Le/num_divisions;
  for(int i=0;i<num_divisions+1;i++){
    float pos = i*step/beam->Le;


  }
}

void setDiagramGlobals(FeaModel* model,BeamElement* beam,int num_divisions){
  float step = beam->Le/num_divisions;
  for(int i=0;i<num_divisions+1;i++){
    float position = i*step/beam->Le;

    float T = calculateDiagramValue(model,beam,position,DIAGRAM_T);
    float MY = calculateDiagramValue(model,beam,position,DIAGRAM_MY);
    float MZ = calculateDiagramValue(model,beam,position,DIAGRAM_MZ);
    float N = calculateDiagramValue(model,beam,position,DIAGRAM_N);
    float VY = calculateDiagramValue(model,beam,position,DIAGRAM_VY);
    float VZ = calculateDiagramValue(model,beam,position,DIAGRAM_VZ);

    if(T > MAX_TORQUE_MX) MAX_TORQUE_MX = T;
    if(T < MIN_TORQUE_MX) MIN_TORQUE_MX = T;

    if(MY > MAX_MOMENT_MY) MAX_MOMENT_MY = MY;
    if(MY < MIN_MOMENT_MY) MIN_MOMENT_MY = MY;

    if(MZ > MAX_MOMENT_MZ) MAX_MOMENT_MZ = MZ;
    if(MZ < MIN_MOMENT_MZ) MIN_MOMENT_MZ = MZ;

    if(VY > MAX_SHEAR_VY) MAX_SHEAR_VY = VY;
    if(VY < MIN_SHEAR_VY) MIN_SHEAR_VY = VY;

    if(VZ > MAX_SHEAR_VZ) MAX_SHEAR_VZ = VZ;
    if(VZ < MIN_SHEAR_VZ) MIN_SHEAR_VZ = VZ;

    if(N > MAX_AXIAL_FORCE) MAX_AXIAL_FORCE = N;
    if(N < MIN_AXIAL_FORCE) MIN_AXIAL_FORCE = N;
  }
  
}

void setFeaGlobals(FeaModel* model){
  
  int num_divisions = 20;
  for(int i=0;i<model->num_beam_elements;i++){
    setDisGlobals(model,&(model->beams[i]),num_divisions);
    setDiagramGlobals(model,&(model->beams[i]),num_divisions);
  }
}
