#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <math.h>
#include <algorithm>
using namespace std;

int euclidian(int x,int i,int g_i) {
  return (x-i)*(x-i) + (g_i*g_i);
}
int sep(int i,int u,int g_i,int g_u) {
  return floor((u*u-i*i + g_u*g_u-g_i*g_i)/(2*(u-i)));
}

int main(){
  int rowsize = 0;
  int columnsize = 0;
  string palabra;
  string letra;
  ifstream inFile;
  inFile.open("data.txt");
  while(inFile >> palabra){rowsize++;}
  inFile.close();
  inFile.open("data.txt");
  inFile >> palabra;
  columnsize = palabra.size();
  cout << rowsize << " "<<columnsize<<"\n";
  int matriz[rowsize][columnsize];
  int semimatriz[rowsize][columnsize];
  int finalmatriz[rowsize][columnsize];
  inFile.close();
  inFile.open("data.txt");
  for(int i = 0; i < rowsize; i++){
    inFile >> palabra;
    for(int j = 0; j < columnsize; j++){
      letra = palabra[j];
      matriz[i][j] = stoi(letra);
    }
  }

  for(int i=0;i<rowsize;i++){
    for(int j=0;j<columnsize;j++){
      cout<<matriz[i][j]<<" ";
    }
    cout<<endl;
  }

  //Fase Uno
  for(int x = 0; x < rowsize; x++){
    if(matriz[x][0] == 0){
      semimatriz[x][0] = 0;
    }
    else{
      semimatriz[x][0] = rowsize+columnsize;
    }
    for(int y = 1; y < columnsize; y++){
      if(matriz[x][y] == 0){
        semimatriz[x][y] = 0;
      }
      else{
        semimatriz[x][y] = 1 + semimatriz[x][y-1];
      }
    }
    for(int y = columnsize-2; y >= 0; y--){
      if(semimatriz[x][y+1]<semimatriz[x][y]){
        semimatriz[x][y] = (1+semimatriz[x][y+1]);
      }
    }
  }
  cout<<endl;
  for(int i=0;i<rowsize;i++){
    for(int j=0;j<columnsize;j++){
      cout<<semimatriz[i][j]<<" ";
    }
    cout<<endl;
  }

  //Fase dos
  int w;
  int q;
  int s[columnsize];
  int t[columnsize];
  for(int y = 0; y < columnsize; y++){
    q = 0;
    s[0] = 0;
    t[0] = 0;
    for(int u = 1; u < rowsize; u++){
      while(q >= 0 &&
        euclidian(t[q],s[q],semimatriz[s[q]][y]) > euclidian(t[q],u,semimatriz[u][y]))
        q --;

      if(q < 0){
        q = 0;
        s[0] = u;
      }
      else{
        w = 1 + sep(s[q],u,semimatriz[s[q]][y],semimatriz[u][y]);
        if(w < rowsize){
          q ++;
          s[q] = u;
          t[q] = w;
        }
      }
    }
    for(int u = rowsize-1; u >= 0; u--){
      finalmatriz[u][y] = floor(sqrt(euclidian(u,s[q],semimatriz[s[q]][y])));
      if(u == t[q]){
        q--;
      }
    }
  }
  cout<<endl;
  for(int i=0;i<rowsize;i++){
    for(int j=0;j<columnsize;j++){
      cout<<finalmatriz[i][j]<<" ";
    }
    cout<<endl;
  }

  ofstream onFile;
  onFile.open("matriz.txt");
  string temp = to_string(columnsize)+" "+to_string(rowsize)+"\n";
  onFile << temp;
  temp = "";
  for(int i = 0; i < rowsize; i++){
    for(int j = 0; j < columnsize; j++){
      temp += to_string(finalmatriz[i][j])+"\t\t";
    }
    temp+="\n";
    onFile << temp;
    temp = "";
  }
  onFile.close();
  return 0;
}
