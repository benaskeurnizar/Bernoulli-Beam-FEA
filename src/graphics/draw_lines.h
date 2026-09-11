#pragma once

#include "graphics.h"
#include "../vulkan_backend/vk_buffer.h"
#include "../fea/fea_types.h"
#include "../fea/beam_result.h"
#include "renderer.h"

#ifdef __cplusplus
extern "C"{
#endif

void createBeamDiagram(FeaModel* model,Renderer* renderer,Diagram* diagram,DIAGRAM_TYPE type);

#ifdef __cplusplus
}
#endif

