#pragma once

#include "graphics.h"
#include "../fea/beam_result.h"
#include "draw_elements.h"

point2D* createCircleOutline(float raduis,float deltaDed,int* count);
point2D* createRectOutline(float width, float height, int segPerEdge, int* count);


Vertex* createMeshedSection(World* world,BeamElement* beam,vec3 center,vec3 forward,vec3 right,vec3 up,float position,uint8_t stresses,int num_divisions,int* vertex_count);
Vertex* connectSectionsLong(Vertex** rings,int num_rings,int ring_size,int* vertex_count);

Vertex* meshInnerRings(World* world,BeamElement* beam,Vertex** rings,int ring_size,int ring_count,int* result_count,vec3 center,float position,uint8_t stresses);

float compute_von_mise(World* world,BeamElement* beam,float y,float z,float position);


float compute_sigmaX(World* world,BeamElement* beam,float y,float z,float position);

float compute_shear(World* world,BeamElement* beam,float y,float z,float position);
