#include "path.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

AppPaths globalPaths = {0};

//TODO: ADD BOUND CHECKING
void initPaths(){
  if(!GetModuleFileNameA(NULL,globalPaths.root,APP_PATH_MAX)){
    fprintf(stderr,"failed to get global paths\n");
    exit(EXIT_FAILURE);
  }
  strcpy(globalPaths.assets,globalPaths.root);
  strcpy(globalPaths.shaders,globalPaths.root);
  // set asset dir
  int root_len = strlen(globalPaths.root);
  int sep_count = 0;
  int asset_index = 0,shader_index = 0;;
  for(int i=root_len-1;i>=0;i--){
    if(globalPaths.root[i]==PATH_SEP){
      sep_count++;
    }
    if(sep_count == 2){
      asset_index = i+1;
      shader_index = i+1;
      break;
    }
  }
  strcpy(&(globalPaths.assets[asset_index]),"assets");
  strcpy(&(globalPaths.shaders[shader_index]),"shaders");
}

char* pathJoin(PATH_TYPE type,char* name,int name_size,int* result_size){
  char* result;
  int len;
  switch(type){
  case ASSETS:
    {
      len = strlen(globalPaths.assets);
      result = malloc(sizeof(char)*(len+name_size+2));//NOTE: 2 is for the null terminator and the separator
      strcpy(result,globalPaths.assets);
    }break;
  case SHADERS:
    {
      len = strlen(globalPaths.shaders);
      result = malloc(sizeof(char)*(len+name_size+2));//NOTE: 2 is for the null terminator and the separator
      strcpy(result,globalPaths.shaders);
    }break;
  }
  *result_size = len+name_size+2;
  result[len] = PATH_SEP;
  strcpy(&(result[len+1]),name);
  result[*result_size -1] = '\0';
  return result;
}
