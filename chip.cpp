#if defined(__APPLE__) || defined(__MACOSX)
    #define IMAGEMAGICK_PATH "/usr/local/bin/convert"
#else
    #define IMAGEMAGICK_PATH "/usr/bin/convert"
#endif

#include "src/Image.hpp"
#include "src/ImageExporter.hpp"

typedef brahand::Image<uint> Image;

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <math.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <vector>
#include "src/Vector.hpp"

#define NUM_THREADS 7
using namespace std;
int inf=0;

int euclidian(int x,int i,int g_i) {
  return (x-i)*(x-i) + (g_i*g_i);
}
int sep(int i,int u,int g_i,int g_u) {
  return floor((u*u-i*i + g_u*g_u-g_i*g_i)/(2*(u-i)));
}
void faseuno(Vector matriz,Vector &semimatriz,int primerafila,int ultimafila, int columnsize){
  for(int x = primerafila; x < ultimafila; x++){
    if(matriz.get(x,0) == 0){
      semimatriz.set(0,x,0);
    }
    else{
      semimatriz.set(inf,x,0);
    }
    for(int y = 1; y < columnsize; y++){
      if(matriz.get(x,y) == 0){
        semimatriz.set(0,x,y);
      }
      else{
        semimatriz.set(1 + semimatriz.get(x,y-1),x,y);
      }
    }
    for(int y = columnsize-2; y >= 0; y--){
      if(semimatriz.get(x,y+1)<semimatriz.get(x,y)){
        semimatriz.set(1+semimatriz.get(x,y+1),x,y);
      }
    }
  }
}
void fasedos(Vector semimatriz,Vector &finalmatriz,int primeracolumna, int ultimacolumna, int rowsize){
  int w;
  int q;
  int s[inf];
  int t[inf];
  for(int y = primeracolumna; y < ultimacolumna; y++){
    q = 0;
    s[0] = 0;
    t[0] = 0;
    for(int u = 1; u < rowsize; u++){
      while(q >= 0 && euclidian(t[q],s[q],semimatriz.get(s[q],y)) > euclidian(t[q],u,semimatriz.get(u,y)))
      q --;

      if(q < 0){
        q = 0;
        s[0] = u;
      }
      else{
        w = 1 + sep(s[q],u,semimatriz.get(s[q],y),semimatriz.get(u,y));
        if(w < rowsize){
          q ++;
          s[q] = u;
          t[q] = w;
        }
      }
    }
    for(int u = rowsize-1; u >= 0; u--){
      finalmatriz.set(floor(sqrt(euclidian(u,s[q],semimatriz.get(s[q],y)))),u,y);
      if(u == t[q]){
        q--;
      }
    }
  }
}
int numerothreads(int &filasporthread, int &restante,int  n){
  if(NUM_THREADS>= n){
    filasporthread=1;
    restante=0;
    return n;
  }else{
    restante=n%NUM_THREADS;
    filasporthread=(n-restante)/NUM_THREADS;
    return NUM_THREADS;
  }
};

int main(){
  Image image("examples/image.png");
  int ndimensions, nsize, number, rowsize,columnsize,depthsize;
  vector<int> dimensions;

  dimensions.push_back(image.image_size.width);
  dimensions.push_back(image.image_size.height);
  dimensions.push_back(image.image_size.depth);

  int *dimensizes=dimensions.data();
  Vector matriz(ndimensions,dimensizes);
  Vector semimatriz(ndimensions,dimensizes);
  Vector finalmatriz(ndimensions,dimensizes);

  rowsize=*dimensizes;
  columnsize=*++dimensizes;
  depthsize=*++dimensizes;
  inf=rowsize+columnsize+depthsize;
  matriz.actual=0;
  for(int i = 0; i < rowsize; i++){
    for(int j = 0; j < columnsize; j++){
          matriz.set(image[image.image_size(i,j,0)],i,j) ;
    }
  }

  //Print matriz
  for(int i=0;i<rowsize;i++){
    for(int j=0;j<columnsize;j++){
      cout<<matriz.get(i,j)<<" ";
    }
    cout<<endl;
  }

  //Threads
  int n,diferencia;
  int nthreads=numerothreads(n,diferencia,rowsize);
  thread threads[nthreads];
  int primero=0;
  int ultimo=n;

/*
  //Fase Uno
  for (int i = 0; i < nthreads; ++i) {
    if(diferencia!=0){
      threads[i] = thread(faseuno, matriz,std::ref(semimatriz),primero,ultimo+1,columnsize);//ultimafila+1
      primero+=n+1;
      ultimo+=n+1;
      diferencia--;
    }else{
      threads[i] = thread(faseuno, matriz,std::ref(semimatriz),primero,ultimo,columnsize );//ultimafila
      primero+=n;
      ultimo+=n;
    }
  }

  for (int i = 0; i < nthreads; ++i) {
      threads[i].join();
  }

  //Print semimatriz
  // cout<<endl;
  for(int i=0;i<rowsize;i++){
    for(int j=0;j<columnsize;j++){
      cout<<semimatriz.get(i,j)<<" ";
    }
    cout<<endl;
  }

  //Fase dos
  nthreads=numerothreads(n,diferencia,columnsize);
  thread threads2[nthreads];
  primero=0;
  ultimo=n;
  for (int i = 0; i < nthreads; ++i) {
    if(diferencia!=0){

      threads2[i] = thread(fasedos, semimatriz,std::ref(finalmatriz),primero,ultimo+1,rowsize);//ultimafila+1
      primero+=n+1;
      ultimo+=n+1;
      diferencia--;
    }else{
      threads2[i] = thread(fasedos, semimatriz,std::ref(finalmatriz),primero,ultimo,rowsize );//ultimafila
      primero+=n;
      ultimo+=n;
    }
  }
  for (int i = 0; i < nthreads; ++i) {
      threads2[i].join();
  }

  //Print finalmatriz
  // cout<<endl;
  for(int i=0;i<rowsize;i++){
    for(int j=0;j<columnsize;j++){
      cout<<finalmatriz.get(i,j)<<" ";
    }
    cout<<endl;
  }

  //Save DT
  for(int i=0;i<rowsize;i++){
    for(int j=0;j<columnsize;j++){
      image[image.image_size(i,j,0)]=matriz.get(i,j);
    }
  }*/
  //brahand::VTKExport::grayscale(image, "./", "image");
  return 0;
}
