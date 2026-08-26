#pragma once

#include "fea_types.h"

void printFormFunc(FormFunc F);
void printFormFunctionMat(FormFunc** F,int w,int h);
void fillNi(FormFunc* F,float* values,int values_size);
void negateFormi(FormFunc* F);
FormFunc** createBeamFormMatrixes(BeamElement* bar,int* w,int* h);

void scaleFormFunc(FormFunc* F,float j);

float getFormValue(FormFunc F,float f);

FormFunc* deriveX(FormFunc F);

FormFunc* deriveX2(FormFunc F);



float integrateX(FormFunc F,float x1,float x2);

void copyNi(FormFunc* src,FormFunc* dst);


FormFunc* mulMatVecF(FormFunc** a, int ah, int aw,FormFunc* b, int bh, int* h);
