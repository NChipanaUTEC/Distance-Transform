#ifndef VECTOR_H
#define VECTOR_H

class Vector {

    private:
        int* data;
        int dataSize;

        int dimensions;
        int* dimensionSizes;

    public:
      int actual;
        Vector() : data(nullptr) {};

        Vector(int dimensions, int* dimensionSizes) : dimensions(dimensions), dimensionSizes(dimensionSizes) {
          dataSize=1;
          for (int i = 0; i < dimensions; i++) {
            dataSize*=*dimensionSizes;
            dimensionSizes++;
          }
          data=new int[dataSize];
            // TODO
        }

        void set(int datum, int x, int y){
          int pos=cmp(x,y);
          data[pos]=datum;
        }; // TODO

        int get(int x, int y){
          int pos=cmp(x,y);
          return data[pos];
        }; // TODO

        int cmp(int x, int y){
          int coordinates[3]={x,y,actual};
          int acum=1;
          int sum=coordinates[0];
          for (int i = 1; i < dimensions; i++) {
            acum*=dimensionSizes[i-1];
            sum+=acum*coordinates[i];
          }
          return sum;
        }
};

#endif
