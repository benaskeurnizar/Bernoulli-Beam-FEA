#include "fea_types.h"
#include "form_functions.h"
#include "matrices.h"
#include "beam_solver.h"
#include "beam_result.h"
#include "fea_globals.h"


FormFunc** createBMatrixe(BeamElement* bar,FormFunc** form,int w,int h){
  FormFunc** result = (FormFunc**)malloc(sizeof(FormFunc*)*(h));
  for(int y=0;y<(h);y++){
    result[y] = (FormFunc*)malloc(sizeof(FormFunc)*(w));
    for(int x=0;x<(w);x++){
      result[y][x].size = form[y][x].size;
      result[y][x].n = (float*)malloc(sizeof(float)*result[y][x].size);
      memset(result[y][x].n,0,sizeof(float)*result[y][x].size);
    }
  }
  
  float j = bar->Le;
  // Row 0
  memcpy(result[0][0].n, form[0][0].n, form[0][0].size * sizeof(float));   // form1l,x  -> same
  memcpy(result[0][6].n, form[0][6].n, form[0][6].size * sizeof(float));   // form2l,x  -> same

  // Row 1
  memcpy(result[1][3].n, form[3][3].n, form[3][3].size * sizeof(float));   // form1l,x  (row3,col3 in form)
  memcpy(result[1][9].n, form[3][9].n, form[3][9].size * sizeof(float));   // form2l,x  (row3,col9 in form)

  // Row 2
  memcpy(result[2][2].n, form[2][2].n, form[2][2].size * sizeof(float));   negateFormi(&result[2][2]);  // -form1c,xx
  memcpy(result[2][4].n, form[2][4].n, form[2][4].size * sizeof(float));   negateFormi(&result[2][4]);// form2c,xx  -> same
  memcpy(result[2][8].n, form[2][8].n, form[2][8].size * sizeof(float));   negateFormi(&result[2][8]);  // -form3c,xx
  memcpy(result[2][10].n, form[2][10].n, form[2][10].size * sizeof(float));negateFormi(&result[2][10]); // form4c,xx -> same

  // Row 3
  memcpy(result[3][1].n, form[1][1].n, form[1][1].size * sizeof(float));   negateFormi(&result[3][1]);  // -form1c,xx
  memcpy(result[3][5].n, form[1][5].n, form[1][5].size * sizeof(float));   negateFormi(&result[3][5]);  // -form2c,xx
  memcpy(result[3][7].n, form[1][7].n, form[1][7].size * sizeof(float));   negateFormi(&result[3][7]);  // -form3c,xx
  memcpy(result[3][11].n, form[1][11].n, form[1][11].size * sizeof(float)); negateFormi(&result[3][11]); // -form4c,xx
  

  FormFunc* B00 = deriveX(result[0][0]);
  FormFunc* B31 = deriveX2(result[3][1]);
  FormFunc* B22 = deriveX2(result[2][2]);
  FormFunc* B13 = deriveX(result[1][3]);
  FormFunc* B24 = deriveX2(result[2][4]);
  FormFunc* B35 = deriveX2(result[3][5]);
  FormFunc* B06 = deriveX(result[0][6]);
  FormFunc* B37 = deriveX2(result[3][7]);
  FormFunc* B28 = deriveX2(result[2][8]);
  FormFunc* B19 = deriveX(result[1][9]);
  FormFunc* B2_10 = deriveX2(result[2][10]);
  FormFunc* B3_11 = deriveX2(result[3][11]);

  scaleFormFunc(B00,1/j);
  scaleFormFunc(B31,(1/j)*(1/j));
  scaleFormFunc(B22,(1/j)*(1/j));
  scaleFormFunc(B13,1/j);
  scaleFormFunc(B24,(1/j)*(1/j));
  scaleFormFunc(B35,(1/j)*(1/j));
  scaleFormFunc(B06,1/j);
  scaleFormFunc(B37,(1/j)*(1/j));
  scaleFormFunc(B28,(1/j)*(1/j));
  scaleFormFunc(B19,1/j);
  scaleFormFunc(B2_10,(1/j)*(1/j));
  scaleFormFunc(B3_11,(1/j)*(1/j));

  copyNi(B00, &result[0][0]);
  copyNi(B31, &result[3][1]);
  copyNi(B22, &result[2][2]);
  copyNi(B13, &result[1][3]);
  copyNi(B24, &result[2][4]);
  copyNi(B35, &result[3][5]);
  copyNi(B06, &result[0][6]);
  copyNi(B37, &result[3][7]);
  copyNi(B28, &result[2][8]);
  copyNi(B19, &result[1][9]);
  copyNi(B2_10, &result[2][10]);
  copyNi(B3_11, &result[3][11]);

  free(B00->n);   free(B00);
  free(B31->n);   free(B31);
  free(B22->n);   free(B22);
  free(B13->n);   free(B13);
  free(B24->n);   free(B24);
  free(B35->n);   free(B35);
  free(B06->n);   free(B06);
  free(B37->n);   free(B37);
  free(B28->n);   free(B28);
  free(B19->n);   free(B19);
  free(B2_10->n); free(B2_10);
  free(B3_11->n); free(B3_11);
  return result;
}

float** createStiffnesMatrix(BeamElement* bar,FormFunc** B,int w,int h,int* KW,int* KH){
  int wt = 0;
  int ht = 0;
  FormFunc** B_t =  transposeMatForm(B,w,h,&ht,&wt);
  int wm1,hm1;
  FormFunc** B_t_D = multmatForm_scale(B_t,wt,ht,bar->D,bar->dw,bar->dh,&hm1,&wm1);
  int kh,kw;
  FormFunc** Ke = multmatForm(B_t_D,wm1,hm1,B,w,h,&kh,&kw);
  //TODO: integrate Ke(FormFunc**) to get Ke(float**)
  float j = bar->Le;
  float** result = (float**)malloc(sizeof(float*)*(kh));
  for(int y=0;y<kh;y++){
    result[y] = (float*)calloc(kw,sizeof(float));
    for(int x=0;x<kw;x++){
      result[y][x] = integrateX(Ke[y][x],0.0,1.0)*j;
    }
  }
  *KW = kw;
  *KH = kh;
  return result;
}

float** TMatrix(Node* nodes,BeamElement* bar,int* w,int* h){//NOTE: returns the T matrix for coordinate change
  float lx,ly,lz;
  float mx,my,mz;
  float nx,ny,nz;
  float Le = bar->Le;
  int nd1 = bar->node1;
  int nd2 = bar->node2;
  lx = (nodes[nd2].x - nodes[nd1].x) / Le;
  ly = (nodes[nd2].y - nodes[nd1].y) / Le;
  lz = (nodes[nd2].z - nodes[nd1].z) / Le;

  
  mx = -ly;
  my =  lx;
  mz =  0;
  float mmag = sqrtf(mx*mx + my*my + mz*mz);
  mx/=mmag; my/=mmag; mz/=mmag;

  nx = ly*mz - lz*my;
  ny = lz*mx - lx*mz;
  nz = lx*my - ly*mx;
  
  *w = 12;
  *h = 12;
  float** result = (float**)malloc(sizeof(float*)*(*h));
  int num = 0;
  for(int y=0;y<(*h);y++){
    if(num>2){
      num=0;
    }
    result[y] = (float*)calloc((*w),sizeof(float));
    int idx = y-num;
    if(num == 0){
      result[y][idx] = lx;result[y][idx+1] = ly;result[y][idx+2] = lz;
    }else if(num==1){
      result[y][idx] = mx;result[y][idx+1] = my;result[y][idx+2] = mz;
    }else{
      result[y][idx] = nx;result[y][idx+1] = ny;result[y][idx+2] = nz;
    }
    num++;
  }
  return result;
}

void createElemetsDisLoad(FeaModel* model){
  FormFunc w[4];
  for(int i=0;i<4;i++){
    w[i].n = (float*)calloc(3,sizeof(float));
    w[i].size = 3;
  }
  for(int i=0;i<model->num_forces;i++){
    for(int j=0;j<4;j++){
      memset((w[j].n),0,sizeof(float)*3);
    }
    int element_idx = 0;
    int N_idx = 0;
    if(model->forces[i].type == FORCE_DIS_Z){
      w[2].n[0] = model->forces[i].x;
      w[2].n[1] = model->forces[i].y;
      w[2].n[2] = model->forces[i].z;
      element_idx = model->forces[i].element_id;
    }else if( model->forces[i].type == FORCE_DIS_Y){
      w[1].n[0] = model->forces[i].x;
      w[1].n[1] = model->forces[i].y;
      w[1].n[2] = model->forces[i].z;
      element_idx = model->forces[i].element_id;
    }else{
      continue;
    }
    if(!model->beams[element_idx].dis_load_size){
      model->beams[element_idx].dis_load_size = 12;
      model->beams[element_idx].dis_load = (float*)calloc(model->beams[element_idx].dis_load_size,sizeof(float));
    }
    int Ntw,Nth;
    FormFunc** Nt = transposeMatForm(model->beams[element_idx].N,model->beams[element_idx].Nw,model->beams[element_idx].Nh,&Nth,&Ntw);

    int Feh;
    FormFunc* Fe = mulMatVecF(Nt,Nth,Ntw,w,4, &Feh);
    if( Feh!=12){
      printf("createElemetsDisLoad: Feh dimensions error %d  \n",Feh);
      exit(EXIT_FAILURE);
    }

    for(int j=0;j<Feh;j++){
      model->beams[element_idx].dis_load[j] += integrateX((Fe[j]),0.0,1.0) * model->beams[element_idx].Le;
    }
    
    for(int k=0;k<Nth;k++){
      for(int l = 0;l<Ntw;l++){
	free(Nt[k][l].n);
      }
      free(Nt[k]);
    }
    free(Nt);

    for(int j=0;j<Feh;j++){
      free(Fe[j].n);
    }
    free(Fe);
  }
  for(int i=0;i<4;i++){
    free(w[i].n);
  }
}

void addElementLoads(FeaModel* model,float* L,int l_size){
  int num_dofs = 6;
  for(int i=0;i<model->num_forces;i++){
    int idx = model->forces[i].node_id*num_dofs;
    if(idx+num_dofs>l_size){
       fprintf(stderr,"addElementLoads : bad index\n");
       exit(EXIT_FAILURE);
    }
    if(model->forces[i].type==FORCE){
      L[idx]+=model->forces[i].x;
      L[idx+1]+=model->forces[i].y;
      L[idx+2]+=model->forces[i].z;
    }else if(model->forces[i].type==MOMENT){
      L[idx+3]+=model->forces[i].x;
      L[idx+4]+=model->forces[i].y;
      L[idx+5]+=model->forces[i].z;
    }else if(model->forces[i].type!=FORCE_DIS_Y && model->forces[i].type!=FORCE_DIS_Z){
      fprintf(stderr,"addElementLoads : wrong load type \n");
      exit(EXIT_FAILURE);
    }
  }
  for(int i=0;i<model->num_beam_elements;i++){
    if(!model->beams[i].dis_load_size){
      continue;
    }
    //NOTE: transform dis forces to global coordinates
    int Tw,Th;
    float** T = TMatrix(model->nodes,&(model->beams[i]),&Tw,&Th);
    //printf("T matrix : \n");
    //printMat(T,Tw,Th);

    int Ttw = 0,Tth = 0;
    float** Tt = transposeMat(T,Tw,Th,&Tth,&Ttw);
    int h;
    float* dis_forcesG = mulMatVec(Tt,Ttw,Tth, model->beams[i].dis_load, model->beams[i].dis_load_size,&h);
    if(h!=12){
      printf("ERROR : H MUST BE 12!\n");
    }
    int node1_idx =model->beams[i].node1*num_dofs ;
    for(int j=0;j<6;j++){
      L[node1_idx+j] += dis_forcesG[j];
    }
    int node2_idx =model->beams[i].node2*num_dofs ;
    for(int j=0;j<6;j++){
      L[node2_idx+j] += dis_forcesG[j+6];
    }

    for(int j=0;j<Th;j++){
      free(T[j]);
    }
    free(T);

    for(int j=0;j<Tth;j++){
      free(Tt[j]);
    }
    free(Tt);
    free(dis_forcesG);
  }
}

void calculateElementsLocalForces(FeaModel* model,BeamElement* beam){
  beam->local_forces = mulMatVec(beam->Ke,beam->Kew,beam->Keh,beam->local_dis,beam->local_dis_size,&(beam->local_forces_size));
  if(beam->dis_load_size){
    for(int i=0;i<beam->dis_load_size;i++){
      beam->local_forces[i] -= beam->dis_load[i];
    }
  }
}


void assemble(float** K,int kw,int kh,float** Ke,int keh,int kew,int node1,int node2,int num_dofs){
  //NOTE: int num_dofs = 6  IS PRE SUPPOSED HERE
  int* dof_map = (int*)calloc(num_dofs*2,sizeof(int));
  for(int i=0;i<num_dofs;i++){
    dof_map[i] =node1 *num_dofs+i;
    dof_map[i+num_dofs] = node2*num_dofs+i;
  }
  for(int y=0;y<keh;y++){
    for(int x=0;x<kew;x++){
      K[dof_map[y]][dof_map[x]] += Ke[y][x];
    }
  }
}

float** reductGlobalK(FeaModel* model,float** K,int kw,int kh,int* w,int* h){
  int num_dofs = 6;
  *w = (num_dofs*model->num_nodes);
  *h = (num_dofs*model->num_nodes);
  int reduction = 0;
  for(int i=0;i<model->num_fixes;i++){
    if(model->fixes[i].type & FIX_X){
      reduction++;
    }
    if(model->fixes[i].type & FIX_Y){
      reduction++;
    }
    if(model->fixes[i].type & FIX_Z){
      reduction++;
    }
    if(model->fixes[i].type & FIX_RX){
      reduction++;
    }
    if(model->fixes[i].type & FIX_RY){
      reduction++;
    }
    if(model->fixes[i].type & FIX_RZ){
      reduction++;
    }
  }
  *w -= reduction;
  *h -= reduction;
  float** K_reducted = (float**)malloc(sizeof(float*)*(*h));
  for(int y=0;y<*h;y++){
    K_reducted[y] = (float*)calloc(*w,sizeof(float));
  }
  int y_idx = 0;
  int x_idx = 0;
  for(int y=0;y<kh;y++){
    int current_node_h = y/num_dofs;
    int current_dof_h = y%num_dofs;
    uint8_t fix = 1<<current_dof_h;
    int pass = 0;
    for(int i=0;i<model->num_fixes;i++){
      if(model->fixes[i].node_id==current_node_h && (model->fixes[i].type&fix)){
	pass = 1;
      }
    }
    if(pass){
      continue;
    }
    x_idx = 0;
    for(int x=0;x<kw;x++){
      int current_node_w = x/num_dofs;
      int current_dof_w = x%num_dofs;
      fix = 1<<current_dof_w;
      pass = 0;
      for(int i=0;i<model->num_fixes;i++){
	if(model->fixes[i].node_id==current_node_w && (model->fixes[i].type&fix)){
	  pass = 1;
	}
      }
      if(pass){
	continue;
      }
      K_reducted[y_idx][x_idx++] = K[y][x];
    }
    y_idx++;
  }
  return K_reducted;
}

float* reductGlobalL(FeaModel* model,float* L,int lh,int* h){
  int num_dofs = 6;
  *h = (num_dofs*model->num_nodes);
  int reduction = 0;
  for(int i=0;i<model->num_fixes;i++){
    if(model->fixes[i].type & FIX_X){
      reduction++;
    }
    if(model->fixes[i].type & FIX_Y){
      reduction++;
    }
    if(model->fixes[i].type & FIX_Z){
      reduction++;
    }
    if(model->fixes[i].type & FIX_RX){
      reduction++;
    }
    if(model->fixes[i].type & FIX_RY){
      reduction++;
    }
    if(model->fixes[i].type & FIX_RZ){
      reduction++;
    }
  }
  *h -= reduction;
  float* L_reducted = (float*)malloc(sizeof(float)*(*h));

  int y_idx = 0;
  for(int y=0;y<lh;y++){
    int current_node_h = y/num_dofs;
    int current_dof_h = y%num_dofs;
    uint8_t fix = 1<<current_dof_h;
    int pass = 0;
    for(int i=0;i<model->num_fixes;i++){
      if(model->fixes[i].node_id==current_node_h && (model->fixes[i].type&fix)){
	pass = 1;
      }
    }
    if(pass){
      continue;
    }
    L_reducted[y_idx++] = L[y];
  }
  return L_reducted;
}


void beamSolver(FeaModel* model){
  //TODO: INITIALIZE THE GLOBAL STIFNESS MATRIX
  int num_dofs = 6;
  int num_nodes = model->num_beam_elements+1;
  int K_global_w = (num_dofs*num_nodes);
  int K_global_h = (num_dofs*num_nodes);
  float** K_global = (float**)malloc(sizeof(float*)*K_global_h);
  for(int y=0;y<K_global_h;y++){
    K_global[y] = (float*)calloc(K_global_w,sizeof(float));
  }
  
  for(int i=0;i<model->num_beam_elements;i++){
    //NOTE: CREATE FORM FUNCTIONS
    int Nw =0,Nh =0;
    model->beams[i].N = createBeamFormMatrixes(&(model->beams[i]),&(model->beams[i].Nw),&(model->beams[i].Nh));
    //printf("N matrix : \n");
    //printFormFunctionMat( model->beams[i].N,(model->beams[i].Nw),(model->beams[i].Nh));

    //NOTE: CREATE B MATRIX
    int Bw = model->beams[i].Nw,Bh = model->beams[i].Nh;
    model->beams[i].B = createBMatrixe(&(model->beams[i]),model->beams[i].N,Bw,Bh);
    //printf("B matrix : \n");
    //printFormFunctionMat( model->beams[i].B,(model->beams[i].Nw),(model->beams[i].Nh));

    //NOTE: CREATE ELEMENT LOCAL STIFFNESS MATRIX
    int Kew,Keh;
    model->beams[i].Ke = createStiffnesMatrix(&(model->beams[i]),model->beams[i].B,Bw,Bh,&(model->beams[i].Kew),&(model->beams[i].Keh));
    Kew = (model->beams[i].Kew);
    Keh = (model->beams[i].Keh);

    //NOTE: CREATE ELEMENT TRANSFORMATION MATRIX
    int Tw,Th;
    float** T = TMatrix(model->nodes,&(model->beams[i]),&Tw,&Th);
    //printf("T matrix : \n");
    //printMat(T,Tw,Th);

    int Ttw = 0,Tth = 0;
    float** Tt = transposeMat(T,Tw,Th,&Tth,&Ttw);
    printf("Tt matrix : \n");
    printMat(Tt,Ttw,Tth);
    
    int m1h,m1w;
    float** m1 =  multmat(Tt,Ttw,Tth,model->beams[i].Ke,Kew,Keh,&m1h,&m1w);
    printf("m1 matrix : \n");
    printMat(m1,m1w,m1h);

    //NOTE: GET ELEMENT GLOBAL STIFFNESS MATRIX
    int Kgw,Kgh;
    float** Kg = multmat(m1,m1w,m1h,T,Tw,Th,&Kgh,&Kgw);
    printf("Kg matrix : \n");
    printMat(Kg,Kgw,Kgh);
    
    //NOTE: ASSEMBLE ELEMENT'S GLOBAL STIFFNESS MATRIX INTO THE MODEL GLOBAL STIFFNESS MATRIX
    assemble(K_global,K_global_w,K_global_h,Kg,Kgh,Kgw,model->beams[i].node1,model->beams[i].node2,num_dofs);

    //FREE
    for(int y=0;y<Kgh;y++){
      free(Kg[y]);
    }
    free(Kg);

    for(int y=0;y<m1h;y++){
      free(m1[y]);
    }
    free(m1);

    for(int y=0;y<Tth;y++){
      free(Tt[y]);
    }
    free(Tt);
  }
  createElemetsDisLoad(model);
   int Lh = num_dofs*num_nodes;
  float*L = (float*)calloc(Lh,sizeof(float));
  addElementLoads(model,L,Lh);
  //printf("K_global matrix : \n");
  //printMat(K_global,K_global_w,K_global_h);

  //TODO: add boundary conditions (modify Global K)
  //NOTE: for now,(and yes still with the hardcoded shit) i will assume node 0 is fixed (encastrement)
  int reducted_w ,reducted_h ;
  //float** K_reducted = getReductedK(K_global,K_global_w,K_global_h,node_start,num_fixed,num_nodes,&reducted_w,&reducted_h);
  //float* L_reducted = getLReducted(L,Lh,node_start,num_fixed,num_nodes,&reducted_h);

  float** K_reducted = reductGlobalK(model,K_global,K_global_w,K_global_h,&reducted_w,&reducted_h);
  float* L_reducted = reductGlobalL(model,L,Lh,&reducted_h);
  //printf("K_reduced matrix : \n");
  //printMat(K_reducted,reducted_w,reducted_h);

  printf("L  : \n");
  printf("{");
  for(int i=0;i<Lh;i++){
    printf(" %f",L[i]);
  }
  printf("}\n");

  printf("L reducted : \n");
  printf("{");
  for(int i=0;i<reducted_h;i++){
    printf(" %f",L_reducted[i]);
  }
  printf("}\n");
  //NOTE: now we have the K and F (HOPEFULLY) and redcuted (ALSO HOPEFULLY), so we just get a,the displacement (is it global or local...i guess global)
  float* Solved = (float*)calloc(reducted_h,sizeof(float));
  gauss_eliminate(K_reducted,L_reducted,Solved,reducted_w,reducted_h);//K*U = F;
  //TODO: now "Solved" has the displacements
  
  printf("Result : \n");
  printf("{");
  for(int i=0;i<reducted_h;i++){
    printf(" %f",Solved[i]);
  }
  printf("}\n");

  int Global_dis_h = K_global_h;
  float* Global_Displacements = (float*)calloc(sizeof(float),Global_dis_h);
  int G_idx = 0,S_idx = 0;
  int y_idx = 0;
  for(int y=0;y<Global_dis_h;y++){
    int current_node_h = y/num_dofs;
    int current_dof_h = y%num_dofs;
    uint8_t fix = 1<<current_dof_h;
    int pass = 0;
    for(int i=0;i<model->num_fixes;i++){
      if(model->fixes[i].node_id==current_node_h && (model->fixes[i].type&fix)){
	pass = 1;
      }
    }
    if(pass){
      G_idx++;
      continue;
    }
    Global_Displacements[G_idx++] = Solved[S_idx++];
    
  }
  printf("node2 global w = %f\n", Global_Displacements[6*2+2]); // node index 2, DOF 2 (w)
  printf("Global Displacements : \n");
  printf("{");
  for(int i=0;i<Global_dis_h;i++){
    printf(" %f",Global_Displacements[i]);
  }
  printf("}\n");
  

  //TODO: now for each element,we comput it's local displacements and then it's F vector to get STRESS RESULTANTS
  for(int i=0;i<model->num_beam_elements;i++){
    int Tw,Th;
    float** T = TMatrix(model->nodes,&(model->beams[i]),&Tw,&Th);
    int temp_size = 6*2;
    float* temp = (float*)calloc(sizeof(float),temp_size);
    int node1 = model->beams[i].node1;
    int node2 = model->beams[i].node2;
    int temp_idx = 0;
    for(int j=node1*6;j<node1*6+6;j++){
      temp[temp_idx++] = Global_Displacements[j];
    }
    for(int j=node2*6;j<node2*6+6;j++){
      temp[temp_idx++] = Global_Displacements[j];
    }
    //NOTE: now temp has his element's 12 dofs from the full global displacement vector
    model->beams[i].local_dis_size = temp_size;
    model->beams[i].local_dis =  mulMatVec(T,Tw,Th,temp,temp_size,&(model->beams[i].local_dis_size));
    printf("element %d local displacements: \n",i);
    printf("{");
    for(int k=0;k<model->beams[i].local_dis_size;k++){
      printf(" %f",model->beams[i].local_dis[k]);
    }
    printf("}\n");

    //NOTE: the local forces
    //model->beams[i].local_forces = mulMatVec(model->beams[i].Ke,model->beams[i].Kew,model->beams[i].Keh,model->beams[i].local_dis,model->beams[i].local_dis_size,&(model->beams[i].local_forces_size));

    /*printf("element %d local forces: \n",i);
    printf("{");
    for(int k=0;k<model->beams[i].local_forces_size;k++){
      printf(" %f",model->beams[i].local_forces[k]);
    }
    printf("}\n");*/

    
    free(temp);
    for(int y=0;y<Th;y++){
      free(T[y]);
    }
    free(T);
  }
  for(int i=0;i<model->num_beam_elements;i++){
    calculateElementsLocalForces(model,&(model->beams[i]));
  }
  printf("local displacements :\n");
  for(int i=0;i<model->num_beam_elements;i++){
    printf("Element %d local displacements : \n",i);
    printf("{");
    for(int j=0;j<model->beams[i].local_dis_size;j++){
      printf(" %.6f",model->beams[i].local_dis[j]);
    }
    printf("}\n");
  }

  printf("local forces :\n");
  for(int i=0;i<model->num_beam_elements;i++){
    printf("Element %d local forces : \n",i);
    printf("{");
    for(int j=0;j<model->beams[i].local_forces_size;j++){
      printf(" %.6f",model->beams[i].local_forces[j]);
    }
    printf("}\n");
  }

  printf("Bar 1 vertical displacement results : \n");
  float pos = 0.0,step = 0.1;
  float v1 = model->beams[0].local_dis[1];
  float theta_z1 = model->beams[0].local_dis[5];
  float v2 = model->beams[0].local_dis[7];
  float theta_z2 = model->beams[0].local_dis[11];
  while(pos <= model->beams[0].Le){
    float v = get_z_displacement(&(model->beams[0]),pos,v1,theta_z1,v2,theta_z2);
    printf("pos = %f : %.6f  ",pos,v);
    pos+=step;
  }
  //float v = get_z_displacement(&(model->beams[0]),2.0,v1,theta_z1,v2,theta_z2);
  //printf("pos = 2.0 : %.6f  ",v);

  //NOTE: get reactions
  int reactions_h = 0;
  float* F_Reactions = mulMatVec(K_global,K_global_w,K_global_h,Global_Displacements,Global_dis_h,&reactions_h);
  printf("REACTIONS : \n");
  printf("{ ");
  for(int i=0;i<reactions_h;i++){
    printf("%f ,",F_Reactions[i]-L[i]);
  }
  printf(" }");
  //return NULL;

  setFeaGlobals(model);
}


