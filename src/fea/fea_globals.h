#pragma once
#include "fea_types.h"


extern float MAX_DEFORM_X,MIN_DEFORM_X;
extern float MAX_DEFORM_Y,MIN_DEFORM_Y;
extern float MAX_DEFORM_Z,MIN_DEFORM_Z;

extern float MAX_ROT_X ,MIN_ROT_X;
extern float MAX_ROT_Y ,MIN_ROT_Y;
extern float MAX_ROT_Z ,MIN_ROT_Z ;


// Stresses
extern float MAX_AXIAL_STRESS;
extern float MIN_AXIAL_STRESS;

extern float MAX_BEND_STRESS_Y;
extern float MIN_BEND_STRESS_Y;

extern float MAX_BEND_STRESS_Z;
extern float MIN_BEND_STRESS_Z;

extern float MAX_SHEAR_STRESS;
extern float MIN_SHEAR_STRESS;

extern float MAX_VON_MISES;
extern float MIN_VON_MISES;

// Diagrams
extern float MAX_AXIAL_FORCE;
extern float MIN_AXIAL_FORCE;

extern float MAX_SHEAR_VY;
extern float MIN_SHEAR_VY;

extern float MAX_SHEAR_VZ;
extern float MIN_SHEAR_VZ;

extern float MAX_MOMENT_MY;
extern float MIN_MOMENT_MY;

extern float MAX_MOMENT_MZ;
extern float MIN_MOMENT_MZ;

extern float MAX_TORQUE_MX;
extern float MIN_TORQUE_MX;


void setFeaGlobals(FeaModel* model);
