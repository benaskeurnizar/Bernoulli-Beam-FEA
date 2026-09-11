#pragma once

#include "graphics.h"
//#include "../vulkan_backend/vk_buffer.h"
#include "../fea/fea_types.h"
#include "../fea/beam_result.h"


typedef enum{
  NO_STRESS = 0,
  SIGMA_X = 1 << 0,
  TOU = 1 << 1,
  VON_MISE = 1 << 2
}STRESS_TYPE;

typedef enum{
  NO_DIS = 0,
  DIS_X = 1 << 0,
  DIS_Y = 1 << 1,
  DIS_Z = 1 << 2,
  ROT_X = 1 << 3,
  ROT_Y = 1 << 4,
  ROT_Z = 1 << 5
}DISPLACEMENT_TYPE;

#ifdef __cplusplus
extern "C" {
#endif

void createElementVertices(FeaModel* model,BeamElement* beam,Mesh* mesh,uint32_t render_flags);

void applyDeformations(FeaModel* model,BeamElement* beam,float position,vec3 center,vec3 forward,vec3 up,vec3 right,uint32_t render_flags);

#ifdef __cplusplus
}
#endif
  
