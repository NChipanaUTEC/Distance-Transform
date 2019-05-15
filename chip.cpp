#if defined(__APPLE__) || defined(__MACOSX)
    #define IMAGEMAGICK_PATH "/usr/local/bin/convert"
#else
    #define IMAGEMAGICK_PATH "/usr/bin/convert"
#endif

#include "src/Image.hpp" //cImage
#include "src/ImageExporter.hpp" //cImage

typedef brahand::Image<uint> Image;

#include <iostream>
#include <thread>
#include <mutex>
#include "src/Vector.hpp" //uniqueArrayImplementation

#define NUM_THREADS 8
using namespace std;

int inf=0;

int euclidian(int x, int i, int gI) {
  return (x-i)*(x-i) + (gI*gI);
}

int sep(int i,int u,int g_i,int g_u) {
  return (u*u-i*i + g_u*g_u-g_i*g_i)/(2*(u-i));
}

void faseUno(Image image, Image &semimatriz, int primeraFila, int ultimaFila, int columnSize, int z){
  for(int y = primeraFila; y < ultimaFila; y++) {
    if(image[image.image_size(y,0,z)] == 0) {
      semimatriz[semimatriz.image_size(y,0,z)]=0;
    }
    else{
      semimatriz[semimatriz.image_size(y,0,z)]=inf;
    }
    for(int x = 1; x < columnSize; x++) {
      if(image[image.image_size(y,x,z)] == 0) {
              semimatriz[semimatriz.image_size(y,x,z)]=0;
      }
      else{
              semimatriz[semimatriz.image_size(y,x,z)]=1 + semimatriz[semimatriz.image_size(y,x-1,z)];
      }
    }
    for(int x = columnSize-2; x >= 0; x--) {
      if(semimatriz[semimatriz.image_size(y,x+1,z)]<semimatriz[semimatriz.image_size(y,x,z)]) {
              semimatriz[semimatriz.image_size(y,x,z)]=1+semimatriz[semimatriz.image_size(y,x+1,z)];
      }
    }
  }
}

void fasedos(Image semimatriz,Image &finalmatriz,int primeracolumna, int ultimacolumna, int rowsize, int z){
        int w;
        int q;
        int s[inf];
        int t[inf];
        for(int y = primeracolumna; y < ultimacolumna; y++) {
                q = 0;
                s[0] = 0;
                t[0] = 0;
                for(int u = 1; u < rowsize; u++) {
                        while(q >= 0 && euclidian(t[q],s[q],semimatriz[semimatriz.image_size(s[q],y,z)]) > euclidian(t[q],u,semimatriz[semimatriz.image_size(u,y,z)]))
                                q--;

                        if(q < 0) {
                                q = 0;
                                s[0] = u;
                        }
                        else{
                                w = 1 + sep(s[q],u,semimatriz[semimatriz.image_size(s[q],y,z)],semimatriz[semimatriz.image_size(u,y,z)]);
                                if(w < rowsize) {
                                        q++;
                                        s[q] = u;
                                        t[q] = w;
                                }
                        }
                }
                for(int u = rowsize-1; u >= 0; u--) {
                        finalmatriz[finalmatriz.image_size(u,y,z)]=floor(sqrt(euclidian(u,s[q],semimatriz[semimatriz.image_size(s[q],y,z)])));
                        if(u == t[q]) {
                                q--;
                        }
                }
        }
}

void fasetres(Image semimatriz,Image &finalmatriz,int primeraprofundidad, int ultimaprofundidad, int rowsize, int x){
        int w;
        int q;
        int s[inf];
        int t[inf];
        for(int z = primeraprofundidad; z < ultimaprofundidad; z++) {
                q = 0;
                s[0] = 0;
                t[0] = 0;
                for(int u = 1; u < rowsize; u++) {
                        while(q >= 0 && euclidian(t[q],s[q],semimatriz[semimatriz.image_size(x,s[q],z)]) > euclidian(t[q],u,semimatriz[semimatriz.image_size(x,u,z)]))
                                q--;

                        if(q < 0) {
                                q = 0;
                                s[0] = u;
                        }
                        else{
                                w = 1 + sep(s[q],u,semimatriz[semimatriz.image_size(x,s[q],z)],semimatriz[semimatriz.image_size(x,u,z)]);
                                if(w < rowsize) {
                                        q++;
                                        s[q] = u;
                                        t[q] = w;
                                }
                        }
                }
                for(int u = rowsize-1; u >= 0; u--) {
                        finalmatriz[finalmatriz.image_size(x,u,z)]=floor(sqrt(euclidian(u,s[q],semimatriz[semimatriz.image_size(x,s[q],z)])));
                        if(u == t[q]) {
                                q--;
                        }
                }
        }
}

int numerothreads(int &filasporthread, int &restante,int n){
        if(NUM_THREADS>= n) {
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
        Image image("examples/hydrogen");
        // cout<<image.image_size.total()<<endl;
        Image image2(image.image_size);
        Image image3(image.image_size);
        Image image4(image.image_size);
        int rowsize,columnSize,depthsize;

        rowsize=image.image_size.height;
        columnSize=image.image_size.width;
        depthsize=image.image_size.depth;
        inf=rowsize+columnSize+depthsize;
        // for(int y=0; y<rowsize; y++) {
        //         for(int x=0; x<columnSize; x++) {
        //                 cout<<image[image.image_size(x,y,0)]<<" ";
        //         }
        //         cout<<endl;
        // }
        // cout<<endl;

        for(int z = 0; z < depthsize; z++) {
                for(int y = 0; y < rowsize; y++) {
                        for(int x = 0; x < columnSize; x++) {
                                if(image[image.image_size(x,y,z)] > 128) {
                                        image[image.image_size(x,y,z)] = 1;
                                }
                                else{
                                        image[image.image_size(x,y,z)] = 0;
                                }
                        }
                }
        }

        //cout<<"Minimo "<<min<<endl;
        //Print matriz
        // for(int y=0; y<rowsize; y++) {
        //         for(int x=0; x<columnSize; x++) {
        //                 cout<<image[image.image_size(x,y,0)]<<" ";
        //         }
        //         cout<<endl;
        // }
        // cout<<endl;

        //Threads
        int n,diferencia;
        int nthreads=numerothreads(n,diferencia,columnSize);
        thread threads[nthreads];
        int primero=0;
        int ultimo=n;

        //Fase Uno
        for (int z = 0; z < depthsize; z++) {
          for (int i = 0; i < nthreads; ++i) {
            if(diferencia!=0) {
              threads[i] = thread(faseUno, image,std::ref(image2),primero,ultimo+1,rowsize,z);//ultimafila+1
              primero+=n+1;
              ultimo+=n+1;
              diferencia--;
            }else{
              threads[i] = thread(faseUno, image,std::ref(image2),primero,ultimo,rowsize,z);//ultimafila
              primero+=n;
              ultimo+=n;
            }
          }
          for (int i = 0; i < nthreads; ++i) {
            threads[i].join();
          }
        }


        //Print semimatriz
        // cout<<endl;
        // for(int y=0; y<rowsize; y++) {
        //         for(int x=0; x<columnSize; x++) {
        //                 cout<<image2[image2.image_size(x,y,0)]<<" ";
        //         }
        //         cout<<endl;
        // }
        // cout<<endl;

        //Fase dos
        nthreads=numerothreads(n,diferencia,rowsize);
        thread threads2[nthreads];
        primero=0;
        ultimo=n;
        for (size_t z = 0; z < depthsize; z++) {
          for (int i = 0; i < nthreads; ++i) {
            if(diferencia!=0) {

              threads2[i] = thread(fasedos, image2,std::ref(image3),primero,ultimo+1,columnSize,z);//ultimafila+1
              primero+=n+1;
              ultimo+=n+1;
              diferencia--;
            }else{
              threads2[i] = thread(fasedos, image2,std::ref(image3),primero,ultimo,columnSize,z);//ultimafila
              primero+=n;
              ultimo+=n;
            }
          }
          for (int i = 0; i < nthreads; ++i) {
            threads2[i].join();
          }
        }

        //Fase tres
        nthreads=numerothreads(n,diferencia,depthsize);
        thread threads3[nthreads];
        primero=0;
        ultimo=n;
        for (size_t x = 0; x < columnSize; x++) {
          for (int i = 0; i < nthreads; ++i) {
                  if(diferencia!=0) {

                          threads3[i] = thread(fasetres, image3,std::ref(image4),primero,ultimo+1,rowsize,x);//ultimafila+1
                          primero+=n+1;
                          ultimo+=n+1;
                          diferencia--;
                  }else{
                          threads3[i] = thread(fasetres, image3,std::ref(image4),primero,ultimo,rowsize,x );//ultimafila
                          primero+=n;
                          ultimo+=n;
                  }
          }
          for (int i = 0; i < nthreads; ++i) {
                  threads3[i].join();
          }
        }


        //Print finalmatriz
        // cout<<endl;
        // for(int y=0; y<rowsize; y++) {
        //         for(int x=0; x<columnSize; x++) {
        //                 cout<<image3[image3.image_size(x,y,0)]<<" ";
        //         }
        //         cout<<endl;
        // }
        // cout<<endl;

        brahand::VTKExport::grayscale(image3, "./", "image");
        return 0;
}
