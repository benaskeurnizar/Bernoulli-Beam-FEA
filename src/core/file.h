#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

uint32_t* align_char_pointer(char* raw_buffer);
void* manual_aligned_alloc(size_t size, size_t allignment);

char* readShaderFile(int* size,const char* address);
