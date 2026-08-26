#include "load_obj.h"
#include "../core/file.h"

//TODO: NOW WE ARE SENDING uint32_t AS INDICES, SO LET VULKAN KNOW!

typedef enum{
  VERTEX,
  TEXTURE,
  NORMAL,
  FACE,
  UNKNOWN,
}lineType;

lineType getLineType(char* line){
  if(!line){
    return UNKNOWN;
  }
  if (line[0] == '\0')
    return UNKNOWN;

  if (line[1] == '\0')
    return UNKNOWN;
  char first = line[0];
  char second = line[1];
  if(first == 'v'){
    if(second == ' '){
      return VERTEX;
    }else if(second == 't'){
      return TEXTURE;
    }else if(second == 'n'){
      return NORMAL;
    }
  }else if(first == 'f'){
    if(second == ' '){
      return FACE;
    }
  }
  return UNKNOWN;
}

typedef struct {
  uint32_t v;
  uint32_t vt;
  uint32_t vn;
  int textureHasValue;
} FaceVertex;

FaceVertex* parseFaceLine(char* line,int* result_size){
  int triplets_count = 3;
  FaceVertex* triplets = (FaceVertex*)calloc(triplets_count,sizeof(FaceVertex));
  int triplets_index = 0;
  
  int size = 32;
  char* current_char = (char*)calloc(size,sizeof(char));
  int char_index = 0;

  int line_index=0;

  int in_vertex = 0,in_texture=0,in_normal=0;
  while(line[line_index] != '\0'){
    if((line[line_index] >= 48 && line[line_index] <= 57)){
      if(!in_vertex && !in_texture && !in_normal){
	in_vertex = 1;
      }
      current_char[char_index++] = line[line_index];
    }
    if(line[line_index] == '/' || (char_index >= size-1)){
      if(in_vertex){
	current_char[char_index] = '\0';
	int x = strtol(current_char, NULL, 10);
	memset(current_char,0,size*sizeof(char));
	triplets[triplets_index].v = (uint32_t)x;
	char_index = 0;
	in_vertex = 0;
	in_texture = 1;
      }else if(in_texture){
	if(char_index==0){
	  triplets[triplets_index].textureHasValue = 0;
	}
	memset(current_char,0,size*sizeof(char));
	char_index = 0;
	in_texture = 0;
	in_normal = 1;
      }else if(in_normal){
	current_char[char_index] = '\0';
	int x = strtol(current_char, NULL, 10);
	triplets[triplets_index].vn = (uint32_t)x;
	memset(current_char,0,size*sizeof(char));
	char_index = 0;
	in_vertex = 0;
	in_texture = 0;
	in_normal = 0;
      }
    }
    if(line[line_index] == ' '){
      if(!in_vertex && !in_normal && !in_texture){

      }else{
	if(in_normal){
	  current_char[char_index] = '\0';
	  int x = strtol(current_char, NULL, 10);
	  triplets[triplets_index].vn = (uint32_t)x;
	  memset(current_char,0,size*sizeof(char));
	  char_index = 0;
	  in_vertex = 0;
	  in_texture = 0;
	  in_normal = 0;
	}else if(in_texture){
	  if(char_index==0){
	    triplets[triplets_index].textureHasValue = 0;
	  }
	  in_texture = 0;
	  in_vertex = 0;
	  in_normal = 0;
	}
	triplets_index++;
	if(triplets_index >= triplets_count-1){
	  triplets_count+=3;
	  FaceVertex* tmp = (FaceVertex*)realloc(triplets,triplets_count*sizeof(FaceVertex));
	  if(!tmp){
	    fprintf(stderr,"failed to realloc triplet ! \n");
	    exit(EXIT_FAILURE);
	  }
	  triplets = tmp;
	}
      }
    }
    line_index++;
  }
  current_char[char_index] = '\0';
  int x = strtol(current_char, NULL, 10);
  if(in_vertex){
    triplets[triplets_index].v = (uint32_t)x;
  }else if(in_texture){
    triplets[triplets_index].vt = (uint32_t)x;
  }else{
    triplets[triplets_index].vn = (uint32_t)x;
  }
  triplets_index++;
  free(current_char);
  triplets_count = triplets_index;
  triplets = (FaceVertex*)realloc(triplets,triplets_count*sizeof(FaceVertex));
  if(!triplets){
    fprintf(stderr,"failed to realloc triplet ! \n");
    exit(EXIT_FAILURE);
  }
  *result_size = triplets_count;
  return triplets;
}

float* parseVertexLine(char* line){
  int index = 0;
  int size = 32;
  char* current_char = (char*)calloc(size,sizeof(char));
  int char_index = 0;

  char* end = NULL;

  float* vertex = (float*)calloc(3,sizeof(float));
  int vertex_index = 0;
  while(line[index] != '\0'){
    if(line[index] == '-' || line[index] == '.' || (line[index] >= 48 && line[index] <= 57)){
      current_char[char_index++] = line[index];
    }
    if((line[index] == ' ' && char_index>0) || (char_index >= size-1)){
      current_char[char_index] = '\0';
      float value = strtof (current_char,&end);
      if(vertex_index >= 3){
	free(current_char);
	return vertex;
      }
      vertex[vertex_index++] = value;
      memset(current_char,0,size*sizeof(char));
      char_index = 0;
    }
    index++;
  }
  if(vertex_index < 3){
    if(char_index > 0){
      current_char[char_index] = '\0';
      float value = strtof (current_char,&end);
      vertex[vertex_index++] = value;
    }
  }
  free(current_char);
  return vertex;
}

void resize_array_vec3(vec3** array,int*size,int new_size){
  *size = new_size;
  *array = (vec3*) realloc(*array,*size*sizeof(vec3));
  if(!(*array)){
    fprintf(stderr,"failed to realloc vec3 ! \n");
    exit(EXIT_FAILURE);
  }
}

void addVertex(Mesh* mesh,Vertex vert,uint32_t index,int vertices_index,int indices_index){
  if(vertices_index > mesh->vertexCount-1){
    //TODO: REALLOC vertices
    mesh->vertexCount+=1000;
    Vertex* temp = (Vertex*)realloc(mesh->vertices,mesh->vertexCount*sizeof(Vertex));
    if(!temp){
      fprintf(stderr,"failed to realloc mesh vertices ! \n");
      exit(EXIT_FAILURE);
    }
    mesh->vertices = temp;
  }
  mesh->vertices[vertices_index] = vert;
  if(indices_index > mesh->indexCount-1){
    //TODO: REALLOC INDICES
    mesh->indexCount+=1000;
    uint32_t* temp = (uint32_t*)realloc(mesh->indices,mesh->indexCount*sizeof(uint32_t));
    if(!temp){
      fprintf(stderr,"failed to realloc mesh indices ! \n");
      exit(EXIT_FAILURE);
    }
    mesh->indices = temp;
  }
  mesh->indices[indices_index] = index;
}

Mesh loadObjectFile(const char* address){
  int vertices_count = 1000;
  int normals_count = 1000;
  
  vec3* vertices = (vec3*)calloc(vertices_count,sizeof(vec3));
  vec3* normals = (vec3*)calloc(normals_count,sizeof(vec3));
  FILE* f = fopen(address,"rb");
  if(!f){
    fprintf(stderr,"failed to open the obj file : %s !\n",address);
    exit(EXIT_FAILURE);
  }
  int vertices_index = 0;
  int indices_index = 0;
  int normals_index = 0;
  char line[256];
  vec3 color ;
  color[0] = 0.2;
  color[1] = 0.1;
  color[2] = 0.8;

  //NOTE: ALLOCATE INITIAL MESH
  int mesh_vertices_count = 1000;
  int mesh_indices_count = 1000;
  Mesh mesh = {0};
  mesh.vertexCount = (uint32_t)mesh_vertices_count;
  mesh.vertices = (Vertex*)calloc(mesh.vertexCount,sizeof(Vertex));
  mesh.indexCount = (uint32_t)mesh_indices_count;
  mesh.indices = (uint32_t*)calloc(mesh.indexCount,sizeof(uint32_t));
  uint32_t mesh_vertices_index = 0;
  uint32_t mesh_indices_index = 0;
  while(fgets(line,sizeof(line),f)!=NULL){
    lineType type = getLineType(line);
    switch (type) {
    case VERTEX:
      {
	float* arrv = parseVertexLine(line);
	vec3 pos;
	pos[0] = arrv[0];pos[1] = arrv[1];pos[2] = arrv[2];
	free(arrv);
	if(vertices_index > vertices_count-1){
	  //TODO: REALLOC
	  resize_array_vec3(&vertices,&vertices_count,vertices_count+1000);
	}
	glm_vec_copy(pos,vertices[vertices_index++]);
      }break;
      //vertices[vertices_index++] = pos;
      

    case TEXTURE:
      //TODO: ADD TEXTURES
      break;

    case NORMAL:
      {
      float* arrn = parseVertexLine(line);
      vec3 normal;
      normal[0] = arrn[0];normal[1] = arrn[1];normal[2] = arrn[2];
      free(arrn);
      if(normals_index > normals_count-1){
	//TODO: REALLOC
	resize_array_vec3(&normals,&normals_count,normals_count+1000);
      }
      glm_vec_copy(normal,normals[normals_index++]);
      }break;
    case FACE:
      {
	int num_verts = 0;
	FaceVertex* verts = parseFaceLine(line,&num_verts);
	int anchor = 0;
	for(int i=1;i<num_verts-1;i++){
	  //NOTE: ANCHOR
	  int v_index = verts[anchor].v;
	  int vn_index = verts[anchor].vn;
	  Vertex vert = {0};
	  glm_vec_copy(color,vert.color);
	  glm_vec_copy(vertices[v_index-1],vert.pos);
	  glm_vec_copy(normals[vn_index-1],vert.normal);
	  uint32_t idx = mesh_vertices_index++;
	  addVertex(&mesh, vert, idx, idx, mesh_indices_index++);
	  //NOTE: I
	  v_index = verts[i].v;
	  vn_index = verts[i].vn;
	  glm_vec_copy(color,vert.color);
	  glm_vec_copy(vertices[v_index-1],vert.pos);
	  glm_vec_copy(normals[vn_index-1],vert.normal);
	  idx = mesh_vertices_index++;
	  addVertex(&mesh, vert, idx, idx, mesh_indices_index++);
	  //NOTE: I+1
	  v_index = verts[i+1].v;
	  vn_index = verts[i+1].vn;
	  glm_vec_copy(color,vert.color);
	  glm_vec_copy(vertices[v_index-1],vert.pos);
	  glm_vec_copy(normals[vn_index-1],vert.normal);
	  idx = mesh_vertices_index++;
	  addVertex(&mesh, vert, idx, idx, mesh_indices_index++);
	}
	//TODO: here we set the indexes,and fill normals from they array to the mesh
      }break;

    default:
      break;
    }
  }
  //TODO: TRIM
  mesh.vertexCount = mesh_vertices_index++;
  Vertex* tempv = (Vertex*)realloc(mesh.vertices,mesh.vertexCount*sizeof(Vertex));
  if(!tempv){
    fprintf(stderr,"failed to realloc mesh vertices ! \n");
    exit(EXIT_FAILURE);
  }
  mesh.vertices = tempv;
  
  mesh.indexCount = mesh_indices_index++;
  uint32_t* tempi = (uint32_t*)realloc(mesh.indices,mesh.indexCount*sizeof(uint32_t));
  if(!tempi){
    fprintf(stderr,"failed to realloc mesh vertices ! \n");
    exit(EXIT_FAILURE);
  }
  mesh.indices = tempi;
  glm_mat4_identity(mesh.model);
  return mesh;
}
