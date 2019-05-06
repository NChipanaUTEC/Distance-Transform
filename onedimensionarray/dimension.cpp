#include <cstdlib>
#include <iostream>

#include "vector.h"

using namespace std;

template <typename T>
struct Dimensions {
    int operator()(int* coordinates, int* sizes, int dimensions) {
      int acum=1;
      int sum=coordinates[0];
      for (int i = 1; i < dimensions; i++) {
        acum*=sizes[i-1];
        sum+=acum*coordinates[i];
      }
      return sum;
        // TODO
    }
};

struct Integer {
       typedef int T;
       typedef Dimensions<T> Operation;
};

int main(int argc, char *argv[]) {

    int array[4]={2,1,4,5};
    int *dimensizes=array;
    Vector <Integer>vector(4,dimensizes);
    int array2[4]={0,1,0,0};
    int *coordi=array2;
    vector.set(5,array2);
    cout<<vector.get(array2)<<endl;
    //system("pause");
    return EXIT_SUCCESS;
}
