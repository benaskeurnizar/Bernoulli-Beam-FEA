#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>



typedef enum{
  FORCE,
  MOMENT,
  FORCE_DIS_Y,
  FORCE_DIS_Z
}Load_Type;

typedef enum {
  FIX_X  = 1 << 0,
  FIX_Y  = 1 << 1,
  FIX_Z  = 1 << 2,
  FIX_RX = 1 << 3,
  FIX_RY = 1 << 4,
  FIX_RZ = 1 << 5,
} Fix_Type;

typedef enum{
  CIRCLE,
  RECTANGLE
}SectionType;

typedef enum{
  BEAM,
}ElementType;

typedef struct{
  float x,y,z;// pos
  int id; // for refrencing
}Node;

typedef struct{
  float* n;
  int size;
}FormFunc;

typedef struct{
  float E,v,p;
  int mat_id;
}Material;

typedef struct{
  float A;       // cross section area
  float Iy;      // moment of inertia around y
  float Iz;      // moment of inertia around z
  float J;       // torsional constant
  int section_id;
  SectionType type;
}Section;

typedef struct{
  int num_nodes;
  int* node_ids; // pointer to the ids of the assigned nodes
  int sec_id;
  int mat_id;
}Element;

typedef struct{
  Load_Type type;
  int node_id;
  int element_id;
  float x,y,z;
}Load;

typedef struct{
  int node_id;
  uint8_t type;
}Fix;

typedef struct{
  int element_id;
  int node1,node2;// node1 and node2 are the ids of the nodes,to acess the nodes array 
  float Le;
  int sec_id;
  int mat_id;
  float** D;
  int dw,dh;
  FormFunc** N;
  FormFunc** B;
  int Nw,Nh;

  //NOTE: TEMP
  float** Ke;
  int Kew,Keh;

  float* dis_load;
  int dis_load_size;
  
  float* local_dis;
  int local_dis_size;

  float* local_forces;
  int local_forces_size;
}BeamElement;

typedef struct{
  Material* materials;
  int num_materials;
  Section* sections;
  int num_sections;
  Node* nodes;
  int num_nodes;
  Load* forces;
  int num_forces;
  Fix* fixes;
  int num_fixes;
  BeamElement* beams;
  int num_beam_elements;

  int fea_solved;
}FeaModel;

