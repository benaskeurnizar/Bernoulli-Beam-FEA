

#include "parse_fea.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "../core/file.h"

void my_error(char* text){
  fprintf(stderr,text);
  exit(EXIT_FAILURE);
}

char** takeSpaces(char* line,int* count){
  if(!line){
    *count = 0;
    return NULL;
  }
  //NOTE: do an expedition and get the actual size
  int size = 0;
  int in_space = 0;
  int in_string = 0;
  int biggest_string = 0;
  int biggest_temp = 0;
  for(int i=0;line[i]!='\0';i++){
    if(line[i] == ' '){
      if(in_space){
	continue;
      }
      in_space = 1;
      if(in_string){
	size++;
	in_string = 0;
	if(biggest_temp > biggest_string){
	  biggest_string = biggest_temp;
	}
	biggest_temp = 0;
      }
    }else{
      if(in_space){
	in_space = 0;
      }
      in_string = 1;
      biggest_temp++;
    }
  }
  if(in_string){
    size++;
    in_string = 0;
    if(biggest_temp > biggest_string){
      biggest_string = biggest_temp;
    }
  }
  *count = size;
  if(size==0){
    *count = 0;
    return NULL;
  }
  char** result = (char**)malloc(sizeof(char*)*size);
  for(int i=0;i<size;i++){
    result[i] = (char*)calloc(biggest_string+1,sizeof(char));
  }
  int result_idx = 0;
  int char_idx = 0;
  in_string = 0;
  in_space = 0;
  for(int i=0;line[i]!='\0';i++){
    if(line[i] == ' '){
      if(in_space){
	continue;
      }
      in_space = 1;
      if(in_string){
	in_string = 0;
	result[result_idx][char_idx++] = '\0';
	char_idx = 0;
	result_idx++;
      }
    }else{
      if(in_space){
	in_space = 0;
      }
      in_string = 1;
      result[result_idx][char_idx++] = line[i];
    }
  }
  if(in_string){
    result[result_idx][char_idx++] = '\0';
  }
  return result;
}

Material* parseMaterials(FILE** f,int* count){
  int initial_count = 5;
  Material* result = (Material*)calloc(initial_count,sizeof(Material));
  int in_material = 0;
  char line[256];
  int idx = 0;
  while(fgets(line,sizeof(line),*f)!=NULL){
    size_t len = strlen(line);
    while(len > 0 && (line[len-1] == '\r' || line[len-1] == '\n')){
      line[--len] = '\0';
    }
    int line_size = 0;
    char** strings = takeSpaces(line,&line_size);
    if(line_size !=2){
      if(line_size==1 && strcmp(strings[0],"END")==0){
	result[idx].mat_id = idx;
	idx++;
	break;
      }
      my_error("MATERIALS : wrong line size \n");
    }
    if(!in_material){
      if(strcmp(strings[0],"MATERIAL")!=0){
	my_error("MATERIALS : identifier \n");
      }
      in_material = 1;
    }else{
      if(strcmp(strings[0],"MATERIAL")==0){
	result[idx].mat_id = idx;
	idx++;
	if(idx>=initial_count){
	  initial_count+=10;
	   Material* temp = (Material*)realloc(result,sizeof(Material)*initial_count);
	  if(!temp){
	    my_error("MATERIALS : failed to realloc \n");
	  }
	  result = temp;
	}
      }else if(strcmp(strings[0],"E")==0){
	float value = strtof (strings[1],NULL);
	result[idx].E = value;
      }else if(strcmp(strings[0],"v")==0){
	float value = strtof (strings[1],NULL);
	result[idx].v = value;
      }else if(strcmp(strings[0],"p")==0){
	float value = strtof (strings[1],NULL);
	result[idx].p = value;
      }else{
	my_error("MATERIALS : bad material value \n");
      }
    }
  }
  *count = idx;
  if(initial_count!=idx){
    Material* temp = (Material*)realloc(result,sizeof(Material)*idx);
    if(!temp){
      my_error("MATERIALS : final realloc failed!\n");
    }
    result = temp;
  }
  return result;
}


Section* parseSections(FILE** f,int* count){
  int initial_count = 5;
  Section* result = (Section*)calloc(initial_count,sizeof(Section));
  int in_section = 0;
  char line[256];
  int idx = 0;
  while(fgets(line,sizeof(line),*f)!=NULL){
    size_t len = strlen(line);
    while(len > 0 && (line[len-1] == '\r' || line[len-1] == '\n')){
      line[--len] = '\0';
    }
    int line_size = 0;
    char** strings = takeSpaces(line,&line_size);
    if(line_size !=2){
      if(line_size==1 && strcmp(strings[0],"END")==0){
	result[idx].section_id = idx;
	idx++;
	break;
      }
      my_error("SECTIONS : wrong line size \n");
    }
    if(!in_section){
      if(strcmp(strings[0],"SECTION")!=0){
	my_error("SECTIONS : BAD identifier \n");
      }
      in_section = 1;
    }else{
      if(strcmp(strings[0],"SECTION")==0){
	result[idx].section_id = idx;
	idx++;
	if(idx>=initial_count){
	  initial_count+=10;
	  Section* temp = (Section*)realloc(result,sizeof(Section)*initial_count);
	  if(!temp){
	    my_error("SECTIONS : failed to realloc \n");
	  }
	  result = temp;
	}
      }else if(strcmp(strings[0],"A")==0){
	float value = strtof (strings[1],NULL);
	result[idx].A = value;
      }else if(strcmp(strings[0],"Iy")==0){
	float value = strtof (strings[1],NULL);
	result[idx].Iy = value;
      }else if(strcmp(strings[0],"Iz")==0){
	float value = strtof (strings[1],NULL);
	result[idx].Iz = value;
      }else if(strcmp(strings[0],"J")==0){
	float value = strtof (strings[1],NULL);
	result[idx].J = value;
      }else if(strcmp(strings[0],"TYPE")==0){
	if(strcmp(strings[1],"CIRCLE")==0){
	  result[idx].type = CIRCLE;
	}else if(strcmp(strings[1],"RECT")==0){
	  result[idx].type = RECTANGLE;
	}else{
	  my_error("SECTIONS : invalid type \n");
	}
      }else{
	my_error("SECTIONS : bad section value \n");
      }
    }
  }
  *count = idx;
  if(initial_count!=idx){
    Section* temp = (Section*)realloc(result,sizeof(Section)*idx);
    if(!temp){
      my_error("SECTIONS : final realloc failed!\n");
    }
    result = temp;
  }
  return result;
}

int checkNodeLine(char** strings,int size){
  if(strcmp(strings[0],"NODE")!=0){
    printf("NODES: Node, error\n");
    return 0;
  }
  char *end;
  long v = strtol(strings[1], &end, 10);
  if(end == strings[1]) {
    printf("NODES: wrong id\n");
    return 0;
  }
  float fv = strtof(strings[2],&end);
  if(end==strings[2]){
    printf("NODES: wrong x pos\n");
    return 0;
  }
  fv = strtof(strings[3],&end);
  if(end==strings[3]){
    printf("NODES: wrong y pos\n");
    return 0;
  }
  fv = strtof(strings[4],&end);
  if(end==strings[4]){
    printf("NODES: wrong z pos\n");
    return 0;
  }
  return 1;
}

Node* parseNodes(FILE** f,int* count){
  int node_count = 0;
  long pos = ftell(*f);
  char line_temp[256];
  while(fgets(line_temp,sizeof(line_temp),*f)){
    size_t len = strlen(line_temp);
    while(len > 0 && (line_temp[len-1] == '\r' || line_temp[len-1] == '\n')){
      line_temp[--len] = '\0';
    }
    int line_size = 0;
    char** strings = takeSpaces(line_temp,&line_size);
    if(line_size!=5){
      if(line_size==1){
	if(strcmp(strings[0],"END")==0){
	  break;
	}else{
	  my_error("NODES: wrong line size\n");
	}
      }else{
	my_error("NODES: wrong line size\n");
      }
    }
    if(!checkNodeLine(strings,line_size)){
      my_error("NODES: bad line!\n");
    }
    node_count++;
  }
  fseek(*f, pos, SEEK_SET);  // rewind back
  Node* result = (Node*)calloc(node_count,sizeof(Node));
  char line[256];
  int idx = 0;
  for(int i=0;i<node_count;i++){
    fgets(line,sizeof(line),*f);
    size_t len = strlen(line);
    while(len > 0 && (line[len-1] == '\r' || line[len-1] == '\n')){
      line[--len] = '\0';
    }
    int line_size = 0;
    char** strings = takeSpaces(line,&line_size);
    char* end;
    result[i].id = i;
    result[i].x = strtof(strings[2],&end);
    result[i].y = strtof(strings[3],&end);
    result[i].z = strtof(strings[4],&end);
    for(int i=0;i<line_size;i++){
      free(strings[i]);
    }
    free(strings);
  }
  *count = node_count;
  //push the f to pass "END"
  fgets(line,sizeof(line),*f);
  return result;
}

int checkElementLine(char** strings,int size){
  if(strcmp(strings[0],"ELEMENT")!=0){
    printf("ELEMENTS: Element, error\n");
    return 0;
  }
  if(strcmp(strings[2],"NODE")!=0){
    printf("ELEMENTS: Element node, error\n");
    return 0;
  }
  if(strcmp(strings[4],"NODE")!=0){
    printf("ELEMENTS: Element node, error\n");
    return 0;
  }
  if(strcmp(strings[6],"SECTION")!=0){
    printf("ELEMENTS: Element section, error\n");
    return 0;
  }
  if(strcmp(strings[8],"MATERIAL")!=0){
    printf("ELEMENTS: Element material, error\n");
    return 0;
  }
  char *end;
  long v = strtol(strings[1], &end, 10);
  if(end == strings[1]) {
    printf("ELEMENTS: {%s} must be a number\n",strings[1]);
    return 0;
  }
  v = strtol(strings[3], &end, 10);
  if(end == strings[3]) {
    printf("ELEMENTS: {%s} must be a number\n",strings[3]);
    return 0;
  }
  v = strtol(strings[5], &end, 10);
  if(end == strings[5]) {
    printf("ELEMENTS: {%s} must be a number\n",strings[5]);
    return 0;
  }
  v = strtol(strings[7], &end, 10);
  if(end == strings[7]) {
    printf("ELEMENTS: {%s} must be a number\n",strings[7]);
    return 0;
  }
  v = strtol(strings[9], &end, 10);
  if(end == strings[9]) {
    printf("ELEMENTS: {%s} must be a number\n",strings[9]);
    return 0;
  }
  return 1;
}

BeamElement* parseElements(FILE** f,int* count,int num_nodes,int num_sections,int num_materials){
  int element_count = 0;
  long pos = ftell(*f);
  char line_temp[256];
  while(fgets(line_temp,sizeof(line_temp),*f)){
    size_t len = strlen(line_temp);
    while(len > 0 && (line_temp[len-1] == '\r' || line_temp[len-1] == '\n')){
      line_temp[--len] = '\0';
    }
    int line_size = 0;
    char** strings = takeSpaces(line_temp,&line_size);
    if(line_size!=10){
      if(line_size==1){
	if(strcmp(strings[0],"END")==0){
	  break;
	}else{
	  my_error("ELEMENTS: wrong line size\n");
	}
      }else{
	my_error("ELEMENTS: wrong line size\n");
      }
    }
    if(!checkElementLine(strings,line_size)){
      my_error("NODES: bad line!\n");
    }
    element_count++;
  }
  fseek(*f, pos, SEEK_SET);  // rewind back
  BeamElement* result = (BeamElement*)calloc(element_count,sizeof(BeamElement));
  char line[256];
  int idx = 0;
  for(int i=0;i<element_count;i++){
    fgets(line,sizeof(line),*f);
    size_t len = strlen(line);
    while(len > 0 && (line[len-1] == '\r' || line[len-1] == '\n')){
      line[--len] = '\0';
    }
    int line_size = 0;
    char** strings = takeSpaces(line,&line_size);
    char* end;
    int node1 = strtol(strings[3],&end,10);
    if(node1 < 0 || node1 > num_nodes){
      my_error("ELEMENTS: out of bounds node1\n");
    }
    result[i].node1 = node1-1;
    int node2 = strtol(strings[5],&end,10);
    if(node2 < 0 || node2 > num_nodes){
      my_error("ELEMENTS: out of bounds node2\n");
    }
    result[i].node2 = node2-1;
    int sec_id = strtol(strings[7],&end,10);
    if(sec_id < 0 || sec_id > num_sections){
      my_error("ELEMENTS: out of bounds section id\n");
    }
    result[i].sec_id = sec_id-1;
    int mat_id = strtol(strings[9],&end,10);
    if(mat_id < 0 || mat_id > num_materials){
      my_error("ELEMENTS: out of bounds material id\n");
    }
    result[i].mat_id = mat_id-1;
    for(int i=0;i<line_size;i++){
      free(strings[i]);
    }
    free(strings);
  }
  *count = element_count;
  //push the f to pass "END"
  fgets(line,sizeof(line),*f);
  return result;
}

int checkPointLoadLine(char** strings,int size){
  if(strcmp(strings[0],"LOAD")!=0){
    printf("LOADS: Load, error\n");
    return 0;
  }
  if(strcmp(strings[1],"NODE")!=0 && strcmp(strings[1],"ELEMENT")!=0){
    printf("LOADS: Load node, error\n");
    return 0;
  }
  if(strcmp(strings[3],"FORCE")!=0 && strcmp(strings[3],"MOMENT")!=0 && strcmp(strings[3],"FORCE_DIS_Z")!=0 && strcmp(strings[3],"FORCE_DIS_Y")!=0){
    printf("LOADS: Load Type, error\n");
    return 0;
  }
  char *end;
  long v = strtol(strings[2], &end, 10);
  if(end == strings[2]) {
    printf("LOADS: {%s} must be a number\n",strings[2]);
    return 0;
  }
  float fv = strtof(strings[4], &end);
  if(end == strings[4]) {
    printf("LOADS: {%s} must be a number\n",strings[4]);
    return 0;
  }
  fv = strtof(strings[5], &end);
  if(end == strings[5]) {
    printf("LOADS: {%s} must be a number\n",strings[5]);
    return 0;
  }
  fv = strtof(strings[6], &end);
  if(end == strings[6]) {
    printf("LOADS: {%s} must be a number\n",strings[6]);
    return 0;
  }
  return 1;
}

int checkDistLoadLine(char** strings,int size){
  if(strcmp(strings[0],"LOAD")!=0){
    printf("LOADS: Load, error\n");
    return 0;
  }
  if(strcmp(strings[1],"NODE")!=0){
    printf("LOADS: Load node, error\n");
    return 0;
  }
  if(strcmp(strings[3],"NODE")!=0 ){
    printf("LOADS: Load Type, error\n");
    return 0;
  }
  if(strcmp(strings[5],"FORCE_DIS_Z")!=0 && strcmp(strings[5],"FORCE_DIS_Y")!=0){
    printf("LOADS: Load Type, error\n");
    return 0;
  }
  char *end;
  long v = strtol(strings[2], &end, 10);
  if(end == strings[2]) {
    printf("LOADS: {%s} must be a number\n",strings[2]);
    return 0;
  }
  v = strtol(strings[4], &end, 10);
  if(end == strings[4]) {
    printf("LOADS: {%s} must be a number\n",strings[2]);
    return 0;
  }
  float fv = strtof(strings[6], &end);
  if(end == strings[6]) {
    printf("LOADS: {%s} must be a number\n",strings[6]);
    return 0;
  }
  fv = strtof(strings[7], &end);
  if(end == strings[7]) {
    printf("LOADS: {%s} must be a number\n",strings[7]);
    return 0;
  }
  fv = strtof(strings[8], &end);
  if(end == strings[8]) {
    printf("LOADS: {%s} must be a number\n",strings[8]);
    return 0;
  }
  return 1;
}

Load* parseLoads(FILE** f,int* count,int num_nodes,int num_elements){
  int load_count = 0;
  long pos = ftell(*f);
  char line_temp[256];
  while(fgets(line_temp,sizeof(line_temp),*f)){
    size_t len = strlen(line_temp);
    while(len > 0 && (line_temp[len-1] == '\r' || line_temp[len-1] == '\n')){
      line_temp[--len] = '\0';
    }
    int line_size = 0;
    char** strings = takeSpaces(line_temp,&line_size);
    if(line_size!=7 ){
      if(line_size==1){
	if(strcmp(strings[0],"END")==0){
	  break;
	}else{
	  my_error("LOADS: wrong line size\n");
	}
      }else{
	my_error("LOADS: wrong line size\n");
      }
    }
    if(!checkPointLoadLine(strings,line_size)){
      my_error("LOADS: bad point load line!\n");
    }
    
    load_count++;
  }
  fseek(*f, pos, SEEK_SET);  // rewind back
  Load* result = (Load*)calloc(load_count,sizeof(Load));
  char line[256];
  int idx = 0;
  for(int i=0;i<load_count;i++){
    fgets(line,sizeof(line),*f);
    size_t len = strlen(line);
    while(len > 0 && (line[len-1] == '\r' || line[len-1] == '\n')){
      line[--len] = '\0';
    }
    int line_size = 0;
    char** strings = takeSpaces(line,&line_size);
    if(strcmp(strings[1],"NODE")==0){
      char* end;
      int node = strtol(strings[2],&end,10);
      if(node<0 || node>num_nodes){
	my_error("LOADS: wrong node\n");
      }
      result[i].node_id = node-1;
      if(strcmp(strings[3],"FORCE")==0){
	result[i].type = FORCE;
      }else if(strcmp(strings[3],"MOMENT")==0){
	result[i].type = MOMENT;
      }else{
	my_error("LOADS: wrong type (point)\n");
      }
      float x = strtof(strings[4],&end);
      float y = strtof(strings[5],&end);
      float z = strtof(strings[6],&end);
      result[i].x = x;
      result[i].y = y;
      result[i].z = z;
    }else if(strcmp(strings[1],"ELEMENT")==0){
      char* end;
      int element = strtol(strings[2],&end,10);
      if(element<0 || element>num_elements){
	my_error("LOADS: wrong element\n");
      }
      result[i].element_id = element-1;
      if(strcmp(strings[3],"FORCE_DIS_Z")==0){
	result[i].type = FORCE_DIS_Z;
      }else if(strcmp(strings[3],"FORCE_DIS_Y")==0){
	result[i].type = FORCE_DIS_Y;
      }else{
	my_error("LOADS: wrong type(distributes)\n");
      }
      float x = strtof(strings[4],&end);
      float y = strtof(strings[5],&end);
      float z = strtof(strings[6],&end);
      result[i].x = x;
      result[i].y = y;
      result[i].z = z;
    }
    for(int i=0;i<line_size;i++){
      free(strings[i]);
    }
    free(strings);
  }
  *count = load_count;
  //push the f to pass "END"
  fgets(line,sizeof(line),*f);
  return result;
}

int checkFixLine(char** strings,int size){
  if(strcmp(strings[0],"FIX")!=0){
    printf("FIXES: Fix, error\n");
    return 0;
  }
  char *end;
  long v = strtol(strings[1], &end, 10);
  if(end == strings[1]) {
    printf("FIXES: {%s} must be a number\n",strings[1]);
    return 0;
  }
  uint8_t a = 0x0;
  for(int i=2;i<size;i++){
    if (strcmp(strings[i], "X") == 0)  a |= 1 << 0;
    if (strcmp(strings[i], "Y") == 0)  a |= 1 << 1;
    if (strcmp(strings[i], "Z") == 0)  a |= 1 << 2;
    if (strcmp(strings[i], "RX") == 0) a |= 1 << 3;
    if (strcmp(strings[i], "RY") == 0) a |= 1 << 4;
    if (strcmp(strings[i], "RZ") == 0) a |= 1 << 5;
  }
  if(a){
    return 1;
  }
  return 0;
}


Fix* parseFixes(FILE** f,int* count,int num_nodes){
  int fix_count = 0;
  long pos = ftell(*f);
  char line_temp[256];
  while(fgets(line_temp,sizeof(line_temp),*f)){
    size_t len = strlen(line_temp);
    while(len > 0 && (line_temp[len-1] == '\r' || line_temp[len-1] == '\n')){
      line_temp[--len] = '\0';
    }
    int line_size = 0;
    char** strings = takeSpaces(line_temp,&line_size);
    if(line_size==1){
      if(strcmp(strings[0],"END")==0){
	break;
      }else{
	my_error("FIXES: wrong line size\n");
      }
    }
    if(line_size > 8){
      my_error("FIXES: wrong line size\n");
    }
    if(!checkFixLine(strings,line_size)){
      my_error("FIXES: bad line!\n");
    }
    fix_count++;
  }
  fseek(*f, pos, SEEK_SET);  // rewind back
  Fix* result = (Fix*)calloc(fix_count,sizeof(Fix));
  char line[256];
  int idx = 0;
  for(int i=0;i<fix_count;i++){
    fgets(line,sizeof(line),*f);
    size_t len = strlen(line);
    while(len > 0 && (line[len-1] == '\r' || line[len-1] == '\n')){
      line[--len] = '\0';
    }
    int line_size = 0;
    char** strings = takeSpaces(line,&line_size);
    char* end;
    int node = strtol(strings[1],&end,10);
    if(node<0 || node>num_nodes){
      my_error("LOADS: wrong node\n");
    }
    result[i].node_id = node-1;
    for(int j=2;j<line_size;j++){
      if(strcmp(strings[j],"X")==0){
	result[i].type |= FIX_X;
      }
      if(strcmp(strings[j],"Y")==0){
	result[i].type |= FIX_Y;
      }
      if(strcmp(strings[j],"Z")==0){
	result[i].type |= FIX_Z;
      }
      if(strcmp(strings[j],"RX")==0){
	result[i].type |= FIX_RX;
      }
      if(strcmp(strings[j],"RY")==0){
	result[i].type |= FIX_RY;
      }
      if(strcmp(strings[j],"RZ")==0){
	result[i].type |= FIX_RZ;
      }
    }
    for(int i=0;i<line_size;i++){
      free(strings[i]);
    }
    free(strings);
  }
  *count = fix_count;
  //push the f to pass "END"
  fgets(line,sizeof(line),*f);
  return result;
}

void parseFea(World* world,const char* address){//TODO: should be void parseFea(World* world,const char* address)
  Material* materials = NULL;
  int material_count = 0;
  Section* sections = NULL;
  int section_count = 0;
  Node* nodes = NULL;
  int node_count = 0;
  BeamElement* elements = NULL;
  int element_count = 0;
  Load* loads = NULL;
  int load_count = 0;
  Fix* fixes = NULL;
  int fix_count = 0;
  FILE* f = fopen(address,"rb");
  if(!f){
    fprintf(stderr,"failed to open the file : %s !\n",address);
    exit(EXIT_FAILURE);
  }
  char line[256];
  fgets(line,sizeof(line),f);
  size_t len = strlen(line);
  while(len > 0 && (line[len-1] == '\r' || line[len-1] == '\n')){
    line[--len] = '\0';
  }
  int first_size = 0;
  char** first_line = takeSpaces(line,&first_size);
  if(!first_line || first_size!=2){
    my_error("bad file! \n");
  }
  if((strcmp(first_line[0],"FEA_FORMAT") != 0) || strcmp(first_line[1],"1")!=0){
    my_error("bad file! \n");
  }
  while(fgets(line,sizeof(line),f)!=NULL){
    size_t len = strlen(line);
    while(len > 0 && (line[len-1] == '\r' || line[len-1] == '\n')){
      line[--len] = '\0';
    }
    int line_size = 0;
    char** strings = takeSpaces(line,&line_size);
    if(line[0]=='#'){
      if(line_size!=2){
	my_error("GLOBAL: wrong line size \n");
      }
      if(strcmp(strings[1],"MATERIALS")==0){
	materials = parseMaterials(&f,&material_count);
	printf("done materials\n");
      }else if(strcmp(strings[1],"SECTIONS")==0){
	sections = parseSections(&f,&section_count);
	printf("done sections\n");
      }else if(strcmp(strings[1],"NODES")==0){
	nodes = parseNodes(&f,&node_count);
	printf("done nodes\n");
      }else if(strcmp(strings[1],"ELEMENTS")==0){
	elements = parseElements(&f,&element_count,node_count,section_count,material_count);
	printf("done elements\n");
      }else if(strcmp(strings[1],"LOADS")==0){
	loads = parseLoads(&f,&load_count,node_count,element_count);
	printf("done loads\n");
      }else if(strcmp(strings[1],"FIXES")==0){
	fixes = parseFixes(&f,&fix_count,node_count);
	printf("done loads\n");
      }
    }
  }
  world->num_materials = material_count;
  world->materials = (Material*)calloc(material_count,sizeof(Material));
  memcpy(world->materials,materials,material_count*sizeof(Material));

  world->num_sections = section_count;
  world->sections = (Section*)calloc(section_count,sizeof(Section));
  memcpy(world->sections,sections,section_count*sizeof(Section));

  world->num_nodes = node_count;
  world->nodes = (Node*)calloc(node_count,sizeof(Node));
  memcpy(world->nodes,nodes,node_count*sizeof(Node));

  world->num_forces = load_count;
  world->forces = (Load*)calloc(load_count,sizeof(Load));
  memcpy(world->forces,loads,load_count*sizeof(Load));

  world->num_fixes = fix_count;
  world->fixes = (Fix*)calloc(fix_count,sizeof(Fix));
  memcpy(world->fixes,fixes,fix_count*sizeof(Fix));

  world->num_beam_elements = element_count;
  world->beams = (BeamElement*)calloc(element_count,sizeof(BeamElement));
  memcpy(world->beams,elements,element_count*sizeof(BeamElement));

  free(materials);
  free(sections);
  free(nodes);
  free(elements);
  free(loads);
  free(fixes);
}

/*

int main(){
  parseFea("sample.txt");
  return 0;
}

*/
