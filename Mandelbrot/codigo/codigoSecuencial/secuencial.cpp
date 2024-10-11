#include <bits/stdc++.h>
#include <fstream>
#include <chrono>
#define MAX_NORM 2
#define MAX_ITER 1000000
using namespace std;

int n, m; // width, heigh
int numPix; 
// limites para x y y 
double x_begin = -1.5, x_end = 0.5; 
double y_begin = -1.0, y_end = 1.0; 
vector<int> result; 

void mandelbrot(double xsize, double ysize){
    for (int i=0; i<numPix; i++){
        pair<double, double> point = {x_begin + (i%n)*xsize, y_begin + (i/m)*ysize};
        pair<double, double> zi, z0 = {0.0, 0.0}; 

        int j = 0; 
        while (j < MAX_ITER){
            zi = {z0.first*z0.first - z0.second*z0.second + point.first, 2*z0.first*z0.second + point.second};
            if (sqrt(zi.first*zi.first + zi.second*zi.second) > MAX_NORM) break;
            z0 = zi; 
            j++;
        }

        // Si hizo todas las iteraciones
        if (j == MAX_ITER) result[i] = 1; 
        else result[i] = 0; 
    }
}

void array_to_pgm( ){
    
    // Hacer imagen 
    ofstream Img ("image-secuencial-1024.pgm");
    if (Img.is_open()){
        Img << "P2\n" << n << " " << m << " 1\n";
        for (int i=0; i<numPix; i++){
            Img << !(result[i]) << " ";
            if (i%n == 0) Img << "\n"; 
        }
        Img.close();
    }
    else{
        cout << "No se pudo hacer la imagen :(" << endl; 
    }
}

int main(){

    n = 1024;
    m = 1024; 
    // numero de pixeles por calcular   
    numPix = n*m;  
    // arreglo para guardar resultado
    result.resize(numPix); 
    
    // tamano de paso para x y y 
    double xsize = fabs(x_begin - x_end)/(n-1); 
    double ysize = fabs(y_begin - y_end)/(m-1); 


    auto start = std::chrono::high_resolution_clock::now();


    mandelbrot( xsize, ysize);
    array_to_pgm( );


    auto stop = std::chrono::high_resolution_clock::now();

    // Calculate the time duration
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Tiempo de ejecucion: " << duration.count()/1e6 << " segundos" << std::endl;





    return 0; 
}