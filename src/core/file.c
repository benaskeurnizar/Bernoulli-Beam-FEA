#include "file.h"

uint32_t* align_char_pointer(char* raw_buffer){
  uintptr_t address = (uintptr_t)raw_buffer;
  while(address%4 != 0){
    address++;
  }
  return (uint32_t*)address;
}

void* manual_aligned_alloc(size_t size, size_t allignment){
  void* raw = malloc(size+allignment-1 + sizeof(void*) );

  uintptr_t alligned_ptr = (uintptr_t)raw + sizeof(void*);//NOTE: leave memory at the front to put old buffer beginning for free
  while(alligned_ptr % allignment !=0){
    alligned_ptr++;
  }

  ((void**)alligned_ptr)[-1] = raw;

  return (void*)alligned_ptr;
  //NOTE: to free do : free(((void**)buf)[-1]);
}

char* readShaderFile(int* size,const char* address){
  FILE* f = fopen(address,"rb");
  if(!f){
    fprintf(stderr,"failed to open the file : %s !\n",address);
    exit(EXIT_FAILURE);
  }
  fseek(f,0,SEEK_END);
  *size = (int)ftell(f);
  fseek(f,0,SEEK_SET);
  if(!(*size)){
    fprintf(stderr,"error while opening file %s , size was 0!\n",address);
    fclose(f);
    exit(EXIT_FAILURE);
  }
  char* result = calloc(*size,sizeof(char));
  if (!result) {
    fprintf(stderr, "out of memory while reading file %s!\n", address);
    fclose(f);
    exit(EXIT_FAILURE);
  }
  size_t read_size = fread((void*)result,sizeof(char),*size,f);
  // Optional check: verify all requested bytes were actually read
  if (read_size != (size_t)*size) {
    fprintf(stderr, "warning: expected %d bytes, but only read %zu from %s\n", *size, read_size, address);
  }
  fclose(f);
  return result;
}

