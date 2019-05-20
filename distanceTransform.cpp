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
#include <string>
#include <mutex>

#define NUM_THREADS 8
using namespace std;

int inf=0;

int euclidean(int x, int i, int gI) {
        return (x-i)*(x-i) + (gI*gI);
}

int sep(int i,int u,int g_i,int g_u) {
        return (u*u-i*i + g_u*g_u-g_i*g_i)/(2*(u-i));
}

void faseUno(Image B, Image &G, int mfirst, int mlast, int nY, int z){
        for(int x = mfirst; x < mlast; x++) {
                if(B[B.image_size(x,0,z)] == 0) {
                        G[G.image_size(x,0,z)]=0;
                }
                else{
                        G[G.image_size(x,0,z)]=inf;
                }
                for(int y = 1; y < nY; y++) {
                        if(B[B.image_size(x,y,z)] == 0) {
                                G[G.image_size(x,y,z)]=0;
                        }
                        else{
                                G[G.image_size(x,y,z)]=1 + G[G.image_size(x,y-1,z)];
                        }
                }
                for(int y = nY-2; y >= 0; y--) {
                        if(G[G.image_size(x,y+1,z)]<G[G.image_size(x,y,z)]) {
                                G[G.image_size(x,y,z)]=1+G[G.image_size(x,y+1,z)];
                        }
                }
        }
}

void faseDos(Image G,Image &DT,int nfirst, int nlast, int mX, int z){
        int w;
        int q;
        int s[mX];
        int t[mX];
        for(int y = nfirst; y < nlast; y++) {
                q = 0;
                s[0] = 0;
                t[0] = 0;
                for(int u = 1; u < mX; u++) {
                        while(q>= 0 && euclidean(t[q],s[q],G[G.image_size(s[q],y,z)])>euclidean(t[q],u,G[G.image_size(u,y,z)]))
                                q--;

                        if(q < 0) {
                                q = 0;
                                s[0] = u;
                        }
                        else{
                                w = 1 + sep(s[q],u,G[G.image_size(s[q],y,z)],G[G.image_size(u,y,z)]);
                                if(w < mX) {
                                        q++;
                                        s[q] = u;
                                        t[q] = w;
                                }
                        }
                }
                for(int u = mX-1; u >= 0; u--) {
                        DT[DT.image_size(u,y,z)]=floor(sqrt(euclidean(u,s[q],G[G.image_size(s[q],y,z)])));
                        if(u == t[q]) {
                                q--;
                        }
                }
        }
}

void faseTres(Image G,Image &DT,int dfirst, int dlast, int nY, int x){
        int w;
        int q;
        int s[inf];
        int t[inf];
        for(int z = dfirst; z < dlast; z++) {
                q = 0;
                s[0] = 0;
                t[0] = 0;
                for(int u = 1; u < nY; u++) {
                        while(q >= 0 && euclidean(t[q],s[q],G[G.image_size(x,s[q],z)]) > euclidean(t[q],u,G[G.image_size(x,u,z)]))
                                q--;

                        if(q < 0) {
                                q = 0;
                                s[0] = u;
                        }
                        else{
                                w = 1 + sep(s[q],u,G[G.image_size(x,s[q],z)],G[G.image_size(x,u,z)]);
                                if(w < nY) {
                                        q++;
                                        s[q] = u;
                                        t[q] = w;
                                }
                        }
                }
                for(int u = nY-1; u >= 0; u--) {
                        DT[DT.image_size(x,u,z)]=floor(sqrt(euclidean(u,s[q],G[G.image_size(x,s[q],z)])));
                        if(u == t[q]) {
                                q--;
                        }
                }
        }
}

void faseCuatro(Image G,Image &DT,int dfirst, int dlast, int mX, int y){
        int w;
        int q;
        int s[inf];
        int t[inf];
        for(int z = dfirst; z < dlast; z++) {
                q = 0;
                s[0] = 0;
                t[0] = 0;
                for(int u = 1; u < mX; u++) {
                        while(q >= 0 && euclidean(t[q],s[q],G[G.image_size(s[q],y,z)]) > euclidean(t[q],u,G[G.image_size(u,y,z)]))
                                q--;

                        if(q < 0) {
                                q = 0;
                                s[0] = u;
                        }
                        else{
                                w = 1 + sep(s[q],u,G[G.image_size(s[q],y,z)],G[G.image_size(u,y,z)]);
                                if(w < mX) {
                                        q++;
                                        s[q] = u;
                                        t[q] = w;
                                }
                        }
                }
                for(int u = mX-1; u >= 0; u--) {
                        DT[DT.image_size(u,y,z)]=floor(sqrt(euclidean(u,s[q],G[G.image_size(s[q],y,z)])));
                        if(u == t[q]) {
                                q--;
                        }
                }
        }
}

void threshold(Image &image,int limit){
        for(int z = 0; z < image.image_size.depth; z++) {
                for(int y = 0; y < image.image_size.height; y++) {
                        for(int x = 0; x < image.image_size.width; x++) {
                                if(image[image.image_size(x,y,z)] > limit) {
                                        image[image.image_size(x,y,z)] = 1;
                                }
                                else{
                                        image[image.image_size(x,y,z)] = 0;
                                }
                        }
                }
        }
}

int maxFunction(Image image){
        int max=0;
        for(int z = 0; z < image.image_size.depth; z++) {
                for(int y = 0; y < image.image_size.height; y++) {
                        for(int x = 0; x < image.image_size.width; x++) {
                                if(max<image[image.image_size(x,y,z)]) {
                                        max=image[image.image_size(x,y,z)];
                                }
                        }
                }
        }
        return max;
}

void printMatriz(Image image, int mX, int nY){
        for(int y=0; y<nY; y++) {
                for(int x=0; x<mX; x++) {
                        cout<<image[image.image_size(x,y,0)]<<" ";
                }
                cout<<endl;
        }
        cout<<endl;
}
int numeroThreads(int &filasporthread, int &restante,int n){
        int nthreads;
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

void parallel(void(*func)(Image,Image&,int,int,int,int) ,int nthreads,Image image, Image &image2, int restante,int perthread,int upper, int nfunc){
  int primero=0;
  int ultimo=perthread;
  int dif=restante;
  thread threads[nthreads];
  for (int z = 0; z < upper; z++) {
    for (int i = 0; i < nthreads; ++i) {
      if(restante!=0) {
              threads[i] = thread(func, image,std::ref(image2),primero,ultimo+1,nfunc,z);//ultimafila+1
              primero+=perthread+1;
              ultimo+=perthread+1;
              restante--;
      }else{
              threads[i] = thread(func, image,std::ref(image2),primero,ultimo,nfunc,z);//ultimafila
              primero+=perthread;
              ultimo+=perthread;
      }
    }
    for (int i = 0; i < nthreads; ++i) {
            threads[i].join();
    }
    primero=0;
    ultimo=perthread;
    restante=dif;
  }
}

int main(){
        Image image("imag.pgm");
        Image image2(image.image_size);
        Image image3(image.image_size);
        Image image4(image.image_size);
        Image image5(image.image_size);
        int mX,nY,dZ;

        mX=image.image_size.width;// x ->mX
        nY=image.image_size.height;//y ->nY
        dZ=image.image_size.depth;// z -> k
        inf=nY+mX+dZ;

        //Threshold
        threshold(image,0);

        //Threads
        int perthread,restante;

        //Fase Uno
        int nthreads=numeroThreads(perthread,restante,mX);
        parallel(faseUno,nthreads,image,image2,restante,perthread,dZ,nY);

        //Fase dos
        nthreads=numeroThreads(perthread,restante,nY);
        parallel(faseDos,nthreads,image2,image3,restante,perthread,dZ,mX);

        //Fase tres
        nthreads=numeroThreads(perthread,restante,dZ);
        parallel(faseTres,nthreads,image3,image4,restante,perthread,mX,nY);

        //Fase cuatro
        nthreads=numeroThreads(perthread,restante,dZ);
        parallel(faseCuatro,nthreads,image4,image5,restante,perthread,nY,mX);

        //Max
        int max=maxFunction(image5);
        brahand::VTKExport::grayscale(image5, "./", "image" ,max);
        return 0;
}
