#include <bits/stdc++.h>
#include <fstream>
#include <mpi.h>
#define MAX_ITER 1000000
#define MAX_NORM 2

using namespace std; 
// CONSTANTES
int n = 1024; // width
int m = 1024; // heigh

int numPix; // numero de pixeles

double x_begin = -1.5, x_end = 0.5; // limites en x 
double y_begin = -1.0, y_end = 1.0; // limites en y
double xsize, ysize; // tamano de paso

// vector de resultados 
vector<int> result;

// Funcion Mandelbrot para calcular si el pixel esta o no en el conjunto
int mandelbrot(int number){

    pair<double, double> point = {x_begin + (number%n)*xsize, y_begin + (number/m)*ysize};
    pair<double, double> zi, z0 = {0.0, 0.0}; 

    int j = 0; 
    while (j < MAX_ITER){
        zi = {z0.first*z0.first - z0.second*z0.second + point.first, 2*z0.first*z0.second + point.second};
        if (sqrt(zi.first*zi.first + zi.second*zi.second) > MAX_NORM) break;
        z0 = zi; 
        j++;
    }

    // Si hizo todas las iteraciones
    if (j == MAX_ITER) return 1; 
    else return 0;
}


// Funcion para guardar imagen 
void array_to_pgm( ){
    ofstream Img("imagenParalelo-1024.pgm");
    if (Img.is_open()){
        Img << "P2\n" << n << " " << m << " 1\n";
        for (int i=0; i<numPix; i++){
            Img << !(result[i]) << " ";
            if (i!=0 && (i+1)%n == 0) Img << "\n";
        }
        Img.close();
    }
    else{
        cout << "No se pudo hacer la imagen :(" << endl; 
    }
}

void runParalelo(int world_rank, int world_size, int numPix){
    // numTask = numPix
    // La idea es cumplir con todas las tareas, que en este caso es el numero de pixeles. 

    int pending_task = 0; 
    int send_task = 0;
    int number = 0;    
    
    // Entramos al proceso maestro
    if (world_rank == 0){

        // Mandamos las primeras tareas 
        for (int i=1; i<world_size; i++){
            number += 1; 
            MPI_Send(&number, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            pending_task++; 
            send_task++; 
        }

        while (true){
            // Recibimos de mensaje de 2 indices: posicion y resultado (esta o no dentro del conjunto)

            int message[2]; 
            MPI_Status status;

            // recibe mensaje
            MPI_Recv(message, 2, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            
            // actualiza los valores 
            result[message[0]] = message[1]; 

            // nos interesa saber cuales procesos ya terminaron
            int free_proc = status.MPI_SOURCE; 

            // mientras tengamos tareas por calcular
            if (send_task < numPix){
                number += 1; 
                // Envia a los trabajadores 
                MPI_Send(&number, 1, MPI_INT, free_proc, 0, MPI_COMM_WORLD);
                send_task++; 
            }else{
                pending_task--;
            }

            // si ya termine, hay que romperlo
            if (pending_task == 0) break; 
        }

        // Generar la imagen 
        array_to_pgm( ); 

        // terminar todo
        int finish = -1;
        for (int i=1; i<world_size; i++){
            MPI_Send(&finish, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    else{
        while(true){
            int number; 
            MPI_Status status; 
            // recibir el indice por calcular 
            MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);    

            // mientras no reciba un indice negativo 
            if (number == -1) break;

            // calcular si esta o no dentro 
            int r = mandelbrot(number); 
            int message[2] = {number, r}; 

            // mandar posicion y resultado            
            MPI_Send(message, 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }
}


int main(int argc, char *argv[]){

    double start = MPI_Wtime(); // funcion para calcular el tiempo

    // calcular tamanos de paso para x y y 
    xsize = fabs(x_begin - x_end)/(n-1); 
    ysize = fabs(y_begin - y_end)/(m-1);
    
    numPix = n*m; // longitud del vector por calcular 
    result.resize(numPix); // memoria para vector 

    // inicializar el proceso paralelo
    MPI_Init(&argc, &argv); 

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Ejecutar funcion en paralelo
    runParalelo(world_rank, world_size, numPix); 
 
    // calcular el tiempo
    double end = MPI_Wtime();
    cout << "El proceso tomo: " << end - start << " segundos para ejecutarse." << endl;

    MPI_Finalize(); 

    return 0;
}


