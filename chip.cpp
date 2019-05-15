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

#define NUM_THREADS 8
using namespace std;
int inf=0;

int euclidian(int x,int i,int g_i) {
        return (x-i)*(x-i) + (g_i*g_i);
}
int sep(int i,int u,int g_i,int g_u) {
        return floor((u*u-i*i + g_u*g_u-g_i*g_i)/(2*(u-i)));
}
void faseuno(Image image,Image &semimatriz,int primerafila,int ultimafila, int columnsize){
        for(int x = primerafila; x < ultimafila; x++) {
                if(image[image.image_size(x,0,0)] == 0) {
                        semimatriz[semimatriz.image_size(x,0,0)]=0;
                }
                else{
                        semimatriz[semimatriz.image_size(x,0,0)]=inf;
                }
                for(int y = 1; y < columnsize; y++) {
                        if(image[image.image_size(x,y,0)] == 0) {
                                semimatriz[semimatriz.image_size(x,y,0)]=0;
                        }
                        else{
                                semimatriz[semimatriz.image_size(x,y,0)]=1 + semimatriz[semimatriz.image_size(x,y-1,0)];
                        }
                }
                for(int y = columnsize-2; y >= 0; y--) {
                        if(semimatriz[semimatriz.image_size(x,y+1,0)]<semimatriz[semimatriz.image_size(x,y,0)]) {
                                semimatriz[semimatriz.image_size(x,y,0)]=1+semimatriz[semimatriz.image_size(x,y+1,0)];
                        }
                }
        }
}
void fasedos(Image semimatriz,Image &finalmatriz,int primeracolumna, int ultimacolumna, int rowsize){
        int w;
        int q;
        int s[inf];
        int t[inf];
        for(int y = primeracolumna; y < ultimacolumna; y++) {
                q = 0;
                s[0] = 0;
                t[0] = 0;
                for(int u = 1; u < rowsize; u++) {
                        while(q >= 0 && euclidian(t[q],s[q],semimatriz[semimatriz.image_size(s[q],y,0)]) > euclidian(t[q],u,semimatriz[semimatriz.image_size(u,y,0)]))
                                q--;

                        if(q < 0) {
                                q = 0;
                                s[0] = u;
                        }
                        else{
                                w = 1 + sep(s[q],u,semimatriz[semimatriz.image_size(s[q],y,0)],semimatriz[semimatriz.image_size(u,y,0)]);
                                if(w < rowsize) {
                                        q++;
                                        s[q] = u;
                                        t[q] = w;
                                }
                        }
                }
                for(int u = rowsize-1; u >= 0; u--) {
                        finalmatriz[finalmatriz.image_size(u,y,0)]=floor(sqrt(euclidian(u,s[q],semimatriz[semimatriz.image_size(s[q],y,0)])));
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
        Image image("fb2.png");
        cout<<image.image_size.total()<<endl;
        Image image2(image.image_size);
        Image image3(image.image_size);
        int rowsize,columnsize,depthsize;

        rowsize=image.image_size.height;
        columnsize=image.image_size.width;
        depthsize=image.image_size.depth;
        inf=rowsize+columnsize+depthsize;
        for(int y=0; y<rowsize; y++) {
                for(int x=0; x<columnsize; x++) {
                        cout<<image[image.image_size(x,y,0)]<<" ";
                }
                cout<<endl;
        }
        cout<<endl;

        for(int z = 0; z < depthsize; z++) {
                for(int y = 0; y < rowsize; y++) {
                        for(int x = 0; x < columnsize; x++) {
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
        for(int y=0; y<rowsize; y++) {
                for(int x=0; x<columnsize; x++) {
                        cout<<image[image.image_size(x,y,0)]<<" ";
                }
                cout<<endl;
        }
        cout<<endl;

        //Threads
        int n,diferencia;
        int nthreads=numerothreads(n,diferencia,columnsize);
        thread threads[nthreads];
        int primero=0;
        int ultimo=n;

        //Fase Uno
        for (int i = 0; i < nthreads; ++i) {
                if(diferencia!=0) {
                        threads[i] = thread(faseuno, image,std::ref(image2),primero,ultimo+1,rowsize);//ultimafila+1
                        primero+=n+1;
                        ultimo+=n+1;
                        diferencia--;
                }else{
                        threads[i] = thread(faseuno, image,std::ref(image2),primero,ultimo,rowsize );//ultimafila
                        primero+=n;
                        ultimo+=n;
                }
        }

        for (int i = 0; i < nthreads; ++i) {
                threads[i].join();
        }

        //Print semimatriz
        // cout<<endl;
        for(int y=0; y<rowsize; y++) {
                for(int x=0; x<columnsize; x++) {
                        cout<<image2[image2.image_size(x,y,0)]<<" ";
                }
                cout<<endl;
        }
        cout<<endl;

        //Fase dos
        nthreads=numerothreads(n,diferencia,rowsize);
        thread threads2[nthreads];
        primero=0;
        ultimo=n;
        for (int i = 0; i < nthreads; ++i) {
                if(diferencia!=0) {

                        threads2[i] = thread(fasedos, image2,std::ref(image3),primero,ultimo+1,columnsize);//ultimafila+1
                        primero+=n+1;
                        ultimo+=n+1;
                        diferencia--;
                }else{
                        threads2[i] = thread(fasedos, image2,std::ref(image3),primero,ultimo,columnsize );//ultimafila
                        primero+=n;
                        ultimo+=n;
                }
        }
        for (int i = 0; i < nthreads; ++i) {
                threads2[i].join();
        }

        //Print finalmatriz
        // cout<<endl;
        for(int y=0; y<rowsize; y++) {
                for(int x=0; x<columnsize; x++) {
                        cout<<image3[image3.image_size(x,y,0)]<<" ";
                }
                cout<<endl;
        }
        cout<<endl;

        //brahand::VTKExport::grayscale(image3, "./", "image");
        return 0;
}
