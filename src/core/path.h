#pragma once

#ifdef _WIN32
    #define PATH_SEP '\\'
#else
    #define PATH_SEP '/'
#endif

#define APP_PATH_MAX 1024

typedef enum{
  ROOT,
  ROOT_EXE,
  ASSETS,
  SHADERS,
}PATH_TYPE;

typedef struct {
    char root[APP_PATH_MAX];
    char assets[APP_PATH_MAX];
    char shaders[APP_PATH_MAX];
} AppPaths;


extern AppPaths globalPaths;

void initPaths();
char* pathJoin(PATH_TYPE type,char* name,int name_size,int* result_size);
