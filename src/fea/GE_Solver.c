

void swap_row(float** a,float* b,int r1,int r2){
  if(r1!=r2){
    float* tempa = a[r1];
    float tempb = b[r1];
    a[r1] = a[r2];
    a[r2] = tempa;
    b[r1] = b[r2];
    b[r2] = tempb;
  }
}

void gauss_eliminate(float** a,float* b,float* x,int width,int height){
  for(int dia=0;dia < height;dia++){
    int max_row = dia;
    float max = a[dia][dia];
    for(int row = dia+1;row<height;row++){
      float temp = a[row][dia];
      if(temp > max){
	max_row = row;
	max = temp;
      }
    }
    swap_row(a,b,dia,max_row);
    for(int row=dia+1;row<height;row++){
      float tmp = a[row][dia]/a[dia][dia];
      for(int col=dia+1;col<width;col++){
	a[row][col] -= tmp*a[dia][col];
      }
      a[row][dia] = 0.0;
      b[row] -= tmp*b[dia];
    }
  }
  for(int row=height-1;row>=0;row--){
    float tmp = b[row];
    for(int j=height-1;j>row;j--){
      tmp -= x[j]*a[row][j];
    }
    x[row] = tmp/a[row][row];
  }
}
