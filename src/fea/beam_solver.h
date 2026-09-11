#pragma once

#include "fea_types.h"
#include "../graphics/graphics.h"
#include "fea_globals.h"

#ifdef __cplusplus
extern "C" {
#endif

void beamSolver(FeaModel* model);
void createElemetsDisLoad(FeaModel* model);

#ifdef __cplusplus
}
#endif


