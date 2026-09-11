#include "fea_types.h"
#include "../parser/parse_fea.h"
#include "path.h"



float** createDMatrix(Material* mat, Section* sec,int* w,int* h){
  *w = 4;
  *h = 4;
  float** D = (float**)malloc(sizeof(float*)*(*h));
  for(int i=0;i<(*h);i++){
    D[i] = (float*)malloc(sizeof(float)*(*w));
    memset(D[i],0,sizeof(float)*(*w));
  }
  float G = mat->E/(2.0*(1+mat->v));
  D[0][0] = mat->E*sec->A;
  D[1][1] = G*sec->J;
  D[2][2] = mat->E*sec->Iz;
  D[3][3] = mat->E*sec->Iy;

  return D;
}

void initFea(FeaModel* model){//TODO: Creates nodes, sections,materials,loads... and bar element
  char sample_name[] = "sample.txt";
  int size;
  char* test_sample_address =  pathJoin(ASSETS,sample_name,strlen(sample_name),&size);
  parseFea(model,test_sample_address);


  for(int i=0;i<model->num_beam_elements;i++){
    model->beams[i].element_id = i;
    int sec_id = model->beams[i].sec_id;
    int mat_id = model->beams[i].mat_id;
    int node1 = model->beams[i].node1;
    int node2 = model->beams[i].node2;
    model->beams[i].D = createDMatrix(&(model->materials[mat_id]),&(model->sections[sec_id]),&(model->beams[i].dw),&(model->beams[i].dh));

    float x1 = model->nodes[node1].x;
    float x2 = model->nodes[node2].x;
    float y1 = model->nodes[node1].y;
    float y2 = model->nodes[node2].y;
    float z1 = model->nodes[node1].z;
    float z2 = model->nodes[node2].z;
    model->beams[i].Le = sqrtf(pow(x2-x1,2)+pow(y2-y1,2)+pow(z2-z1,2));
  }

  free(test_sample_address);
}







//TODO: check for 0 Ns (make sure it's always handeled)
//TODO: double check everything
