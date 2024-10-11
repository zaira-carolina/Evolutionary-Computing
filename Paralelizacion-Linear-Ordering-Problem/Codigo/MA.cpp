/*******************************************************************************************
Authors: Carlos Segura (Programmer and Designer), Lazaro Lugo (Programmer and Designer),
Gara Miranda (Designer)

Description: implementation of MA-EDM. The novelty appears in the replacement phase, where
the BNP strategy is implemented. See "A Diversity-Aware Memetic Algorithm for the Linear
Order Problem" for a complete description. There are some parts that are specific of the
LOP, such as the
********************************************************************************************/

#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <signal.h>
#include <string>
#include <mpi.h>

#include "MA.h"
#include "utils.h"
#define TAG1 1
#define TAG2 2

using namespace std;

MA::MA(int N_, double pc_, string &crossType_, double finalTime_, string &outputFile_)
{
    N = N_;
    pc = pc_;
    crossType = crossType_;
    finalTime = finalTime_;
    outputFile = outputFile_;
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    initialTime = (double)(currentTime.tv_sec) + (double)(currentTime.tv_usec) / 1.0e6;
}

// Initialize and apply intensification to each individual
void MA::initPopulation()
{
    for (int i = 0; i < N; i++)
    {
        Individual *ei = new Individual();
        ei->initialize_heuristic();
        ei->intensify();
        population.push_back(ei);
    }
}

// Select two parents with binary selection
void MA::selectParents()
{
    parents.clear();
    for (int i = 0; i < population.size(); i++)
    {
        int first = getRandomInteger0_N(N - 1);
        int second = getRandomInteger0_N(N - 1);
        if (population[first]->getCost() <= population[second]->getCost())
        {
            parents.push_back(population[first]);
        }
        else
        {
            parents.push_back(population[second]);
        }
    }
}

// Apply crossover: two crossover operators were implemented cx and bx
void MA::crossover()
{
    for (int i = 0; i < parents.size(); i++)
    {
        Individual *ni = new Individual();
        *ni = *parents[i];
        offspring.push_back(ni);
    }

    if (crossType == "cx")
    {
        for (int i = 0; i < offspring.size() - N % 2; i += 2)
        {
            if (generateRandomDouble0_Max(1) <= pc)
            {
                offspring[i]->crossoverCX(*offspring[i + 1]);
            }
        }
    }
    else if (crossType == "ob")
    {
        for (int i = 0; i < offspring.size() - N % 2; i += 2)
        {
            if (generateRandomDouble0_Max(1) <= pc)
            {
                offspring[i]->crossoverOB(*offspring[i + 1]);
            }
        }
    }
    else
    {
        cout << "No se reconoce el tipo de operador de cruce" << endl;
        exit(-1);
    }
}

void MA::intensify()
{
    for (int i = 0; i < offspring.size(); i++)
    {
        offspring[i]->intensify();
    }
}

void MA::replacement()
{
    vector<Individual *> all;

    // Join population and offspring
    for (int i = 0; i < population.size(); i++)
    {
        all.push_back(population[i]);
        all.back()->distance = MAX_DISTANCE;
    }
    population.clear();

    for (int i = 0; i < offspring.size(); i++)
    {
        all.push_back(offspring[i]);
        all.back()->distance = MAX_DISTANCE;
    }
    offspring.clear();

    // Select best solution
    int indexBest = 0;
    for (int i = 1; i < all.size(); i++)
    {
        if (all[i]->getCost() < all[indexBest]->getCost())
        {
            indexBest = i;
        }
    }
    population.push_back(all[indexBest]);
    all[indexBest] = all.back();
    all.pop_back();

    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    double elapsedTime = (double)(currentTime.tv_sec) + (double)(currentTime.tv_usec) / 1.0e6;
    elapsedTime -= initialTime;

    // Select next N - 1 solution
    double D = DI - DI * elapsedTime / finalTime;
    while (population.size() != N)
    {
        // Update distances
        for (int i = 0; i < all.size(); i++)
        {
            all[i]->distance = min(all[i]->distance, all[i]->getDistance(*population.back()));
        }
        // Select best option
        indexBest = 0;
        for (int i = 1; i < all.size(); i++)
        {
            bool betterInDist = (all[i]->distance > all[indexBest]->distance);
            bool eqInDist = (all[i]->distance == all[indexBest]->distance);
            bool betterInFit = (all[i]->getCost() < all[indexBest]->getCost());
            bool eqInFit = (all[i]->getCost() == all[indexBest]->getCost());

            if (all[indexBest]->distance < D)
            { // Do not fulfill distance requirement
                if ((betterInDist) || (eqInDist && betterInFit))
                {
                    indexBest = i;
                }
            }
            else
            {
                if (all[i]->distance >= D)
                {
                    if ((betterInFit) || (eqInFit && betterInDist))
                    {
                        indexBest = i;
                    }
                }
            }
        }
        // Insert best option
        population.push_back(all[indexBest]);
        all[indexBest] = all.back();
        all.pop_back();
    }
    // Release memory
    for (int i = 0; i < all.size(); i++)
    {
        delete (all[i]);
    }
}

void MA::initDI()
{
    double meanDistance = 0;
    for (int i = 0; i < population.size(); i++)
    {
        for (int j = i + 1; j < population.size(); j++)
        {
            meanDistance += population[i]->getDistance(*population[j]);
        }
    }
    meanDistance /= (population.size() * (population.size() - 1)) / 2;
    DI = meanDistance;
}

void Individual::updateData(const std::vector<int> &receivedS, TFitness receivedCost)
{
    // Update S and cost based on received data
    S = receivedS;
    cost = -receivedCost;
}

void MA::intensifySlave(){
    int dim =  offspring[0]->problem->problemDimension; 
    
    while (true){

        MPI_Status status;

        vector<int> recieved_permuta (dim+2); 
        MPI_Recv(recieved_permuta.data(), recieved_permuta.size(), MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        int numero = recieved_permuta.back();
        recieved_permuta.pop_back(); 

        int costo = recieved_permuta.back();
        recieved_permuta.pop_back(); 
        
        if (numero != -1 && numero != -2){

            Individual *ind = new Individual();

            // le actualizo los valores del vector de permutacion y costo
            ind->updateData(recieved_permuta, costo);

            // intensifico
            ind->intensify();

            int data_to_send = ind->getCost();
            vector<int> send_vector = ind->getSVector(); 

            send_vector.push_back(data_to_send);
            send_vector.push_back(numero);

            MPI_Send(send_vector.data(), send_vector.size(), MPI_INT, 0, 0, MPI_COMM_WORLD);

        }
        else if (numero == -1) continue;
        else break; 
    }
}

void MA::run()
{
    initPopulation();
    initDI();
    generation = 0;
    double cTime;
    double bestCost;

    do
    {
        // Iteration of the MA: selection, crossover, intensification, replacement
        selectParents();
        crossover();

        // se debe de mandar la permutacion y el costo
        int number = 0; // para mandar los indices
        int send_task = 0; // para contar cuantos indices se han mandado
        int pending_task = 0; // para ver cuantos nodos faltan por acabar 
        int waitSignal = -1; // senal de espera para la siguiente iteracion
        
        int dim = offspring[0]->problem->problemDimension;  

        for (int i = 1; i < 6; i++){
            // pone el vector de permutaciones
            vector<int> permuta = offspring[number]->getSVector();
            // calculo el costo del vector
            int costo_inicial = offspring[number]->getCost();

            // los pongo al final del vector 
            permuta.push_back(costo_inicial); 
            permuta.push_back(number); 

            // lo mando
            MPI_Send(permuta.data(), permuta.size(), MPI_INT, i, 0, MPI_COMM_WORLD);

            number++;
            send_task++; 
            pending_task++; 
        }

        while (pending_task > 0)
        {

            MPI_Status status;
            vector<int> vector_actualizado(dim+2); // Set 'expected_size' to the actual size of the received data

            MPI_Recv(vector_actualizado.data(), vector_actualizado.size(), MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
     

            int indice_recibido = vector_actualizado.back();
            vector_actualizado.pop_back();

            int costo_actualizado = vector_actualizado.back(); 
            vector_actualizado.pop_back();  


            offspring[indice_recibido]->S = vector_actualizado;

            // actualizar costo
            offspring[indice_recibido]->updateCost(costo_actualizado); 


            int free_process = status.MPI_SOURCE;
            if (number < offspring.size())
            {

                vector<int> permuta = offspring[number]->getSVector(); 
                int data_to_send = offspring[number]->getCost();

                permuta.push_back(data_to_send); 
                permuta.push_back(number); 


                MPI_Send(permuta.data(), permuta.size(), MPI_INT, free_process, 0, MPI_COMM_WORLD);
                number++; 
                send_task++;
            }
            else
           {
                pending_task--;
            }
        }

        vector<int> empty(dim+2); 
        int waiting_signal = -1;  

        empty.back() = waiting_signal;
        for (int i=1; i<6; i++){
            MPI_Send(empty.data(), empty.size(), MPI_INT, i, 0, MPI_COMM_WORLD);
        }


        replacement();
        struct timeval currentTime;
        gettimeofday(&currentTime, NULL);
        cTime = (double)(currentTime.tv_sec) + (double)(currentTime.tv_usec) / 1.0e6;
        elapsedTime = cTime - initialTime;
        if (generation == 0)
        {
            bestCost = population[0]->getCost();
        }
        else
        {
            if (population[0]->getCost() < bestCost)
            {
                bestCost = population[0]->getCost();
            }
        }
        generation++;
    } while (cTime - initialTime < finalTime);
    // print best solution
    population[0]->print(outputFile);
    
    int fin = -2; 

    vector<int> empty (population[0]->problem->problemDimension + 2 );
    empty.back() = fin;

    for (int i=1; i<6; i++){
        MPI_Send(empty.data(), empty.size(), MPI_INT, i, 0, MPI_COMM_WORLD);
    }



}