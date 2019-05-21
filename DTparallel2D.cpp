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
#include <chrono>
#include <pthread.h>
#include <ctime>
#include <mutex>
#include <bits/stdc++.h>

using namespace std;

Image B("examples/yinyang.png");
Image G(B.image_size);
Image DT(B.image_size);

struct Par{
  int inicio;
  int final;
  int mnd;
  Par(int i, int f, int MND){
    inicio=i;
    final=f;
    mnd=MND;
  };
};

pthread_barrier_t barrera;
std::vector<Par*> v;
int inf=0;

int euclidean(int x, int i, int gI) {
        return (x-i)*(x-i) + (gI*gI);
}

int sep(int i,int u,int g_i,int g_u) {
        return (u*u-i*i + g_u*g_u-g_i*g_i)/(2*(u-i));
}

void threshold(Image &image,int limit){
        for(int z = 0; z < image.image_size.depth; z++) {
                for(int y = 0; y < image.image_size.height; y++) {
                        for(int x = 0; x < image.image_size.width; x++) {
                                if(image[image.image_size(x,y,z)] < limit) {
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

void* runThreads(void* th){
  long tid;
   tid = (long)th;
   Par *par=v[tid];
   for(int z=0;z<B.image_size.depth;z++){
     for(int x = par->inicio; x < par->final; x++) {
             if(B[B.image_size(x,0,z)] == 0) {
                     G[G.image_size(x,0,z)]=0;
             }
             else{
                     G[G.image_size(x,0,z)]=inf;
             }
             for(int y = 1; y < par->mnd; y++) {
                     if(B[B.image_size(x,y,z)] == 0) {
                             G[G.image_size(x,y,z)]=0;
                     }
                     else{
                             G[G.image_size(x,y,z)]=1 + G[G.image_size(x,y-1,z)];
                     }
             }
             for(int y = par->mnd-2; y >= 0; y--) {
                     if(G[G.image_size(x,y+1,z)]<G[G.image_size(x,y,z)]) {
                             G[G.image_size(x,y,z)]=1+G[G.image_size(x,y+1,z)];
                     }
             }
     }
   }
   pthread_barrier_wait(&barrera);
   for(int z=0;z<B.image_size.depth;z++){
     int w;
     int q;
     int s[par->mnd];
     int t[par->mnd];
     for(int y = par->inicio; y < par->final; y++) {
             q = 0;
             s[0] = 0;
             t[0] = 0;
             for(int u = 1; u < par->mnd; u++) {
                     while(q>= 0 && euclidean(t[q],s[q],G[G.image_size(s[q],y,z)])>euclidean(t[q],u,G[G.image_size(u,y,z)]))
                             q--;

                     if(q < 0) {
                             q = 0;
                             s[0] = u;
                     }
                     else{
                             w = 1 + sep(s[q],u,G[G.image_size(s[q],y,z)],G[G.image_size(u,y,z)]);
                             if(w < par->mnd) {
                                     q++;
                                     s[q] = u;
                                     t[q] = w;
                             }
                     }
             }
             for(int u = par->mnd-1; u >= 0; u--) {
                     DT[DT.image_size(u,y,z)]=floor(sqrt(euclidean(u,s[q],G[G.image_size(s[q],y,z)])));
                     if(u == t[q]) {
                             q--;
                     }
             }
     }
   }
}
int main(){
        int NUM_THREADS;
        cin>>NUM_THREADS;
        int mX,nY,dZ;

        mX=B.image_size.width;// x ->mX
        nY=B.image_size.height;//y ->nY
        dZ=B.image_size.depth;// z -> k
        inf=nY+mX+dZ;
        //Threshold
        threshold(B,150);
        //Threads
        pthread_t threads[NUM_THREADS];
        int n=mX/NUM_THREADS;
        int r=mX%NUM_THREADS;
        Par *par;
        for(int k=0;k<NUM_THREADS;k++){
          if(k==NUM_THREADS-1){
            par=new Par(k*n,(k+1)*n+r,mX);
          }
          else{
            par=new Par(k*n,(k+1)*n,mX);
          }
          v.push_back(par);
        }
        pthread_barrier_init(&barrera, NULL, NUM_THREADS);

        auto start1 = chrono::high_resolution_clock::now();
        for(long i=0;i<NUM_THREADS;i++){
          pthread_create(&threads[i], NULL, runThreads, (void* )i);
        }
        for(int j=0;j<NUM_THREADS;j++){
          pthread_join(threads[j], NULL);

        }
        auto end1 = chrono::high_resolution_clock::now();
        double time_taken1 = chrono::duration_cast<chrono::nanoseconds>(end1 - start1).count();
        time_taken1 *= 1e-9;
        cout << "Time taken by program is : " << fixed << time_taken1 << setprecision(9)<< " sec" << endl;

        int max=maxFunction(DT);
        brahand::VTKExport::grayscale(DT, "./", "image" ,max);
        return 0;
}
