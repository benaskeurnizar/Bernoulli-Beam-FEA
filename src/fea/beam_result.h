#pragma once

#include "fea_types.h"
#include "../graphics/graphics.h"

float get_v_displacement(BeamElement* bar,float position,float v1,float theta_z1,float v2,float theta_z2);

float get_z_displacement(BeamElement* bar,float position,float v1,float theta_z1,float v2,float theta_z2);
float get_y_displacement(BeamElement* bar,float position,float w1,float theta_y1,float w2,float theta_y2);
float get_x_displacement(BeamElement* bar,float position,float u1,float u2);
float get_x_rotation(BeamElement* bar,float position,float theta_x1,float theta_x2);

float get_My(BeamElement* bar,float position,float E,float Iy);
float get_Mz(BeamElement* bar,float position,float E,float Iz);

float* getSectionStress(World* world,BeamElement* beam,float position,int* size);

float get_z_rotation(BeamElement* bar,float position,float v1,float theta_z1,float v2,float theta_z2);


float get_y_rotation(BeamElement* bar,float position,float w1,float theta_y1,float w2,float theta_y2);
