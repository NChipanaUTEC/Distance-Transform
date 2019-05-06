#ifndef VECTOR_H
#define VECTOR_H

template <typename Tr>
class Vector {
    public:
        typedef typename Tr::T T;
        typedef typename Tr::Operation Operation;

    private:
        T* data;
        int dataSize;

        int dimensions;
        int* dimensionSizes;

        Operation cmp;

    public:

        Vector() : data(nullptr) {};

        Vector(int dimensions, int* dimensionSizes) : dimensions(dimensions), dimensionSizes(dimensionSizes) {
          dataSize=1;
          for (int i = 0; i < dimensions; i++) {
            dataSize*=*dimensionSizes;
            dimensionSizes++;
          }
          data=new T[dataSize];
            // TODO
        }

        void set(T datum, int* coordinates){
          int pos=cmp(coordinates,dimensionSizes,dimensions);
          data[pos]=datum;
        }; // TODO

        T get(int* coordinates){
          int pos=cmp(coordinates,dimensionSizes,dimensions);
          return data[pos];
        }; // TODO
};

#endif
