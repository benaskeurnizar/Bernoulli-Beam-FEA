#pragma once

#include "graphics.h"
#include "../fea/beam_result.h"
#include "draw_elements.h"

point2D* createCircleOutline(float raduis,float deltaDed,int* count);
point2D* createRectOutline(float width, float height, int segPerEdge, int* count);


Vertex* createMeshedSection(FeaModel* model,BeamElement* beam,vec3 center,vec3 forward,vec3 right,vec3 up,float position,uint32_t render_flags,int num_divisions,int* vertex_count);
Vertex* connectSectionsLong(Vertex** rings,int num_rings,int ring_size,int* vertex_count);

Vertex* meshInnerRings(FeaModel* model,BeamElement* beam,Vertex** rings,int ring_size,int ring_count,int* result_count,vec3 center,float position,uint32_t render_flags);

float compute_von_mise(FeaModel* model,BeamElement* beam,float y,float z,float position);


float compute_sigmaX(FeaModel* model,BeamElement* beam,float y,float z,float position);

float compute_shear(FeaModel* model,BeamElement* beam,float y,float z,float position);


float compute_axial_stress(FeaModel* model,BeamElement* beam,float y,float z,float position);

float compute_bend_stress_y(FeaModel* model,BeamElement* beam,float y,float z,float position);

float compute_bend_stress_z(FeaModel* model,BeamElement* beam,float y,float z,float position);
