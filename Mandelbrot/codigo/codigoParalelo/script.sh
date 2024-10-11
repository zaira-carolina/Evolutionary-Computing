#!/bin/bash
#Intrucciones: cambie el grupo en que va a ejecutar, el nombre del trabajo para 
# identificarlo fácilmente, el número de núcleos máximo por nodo, 
# el número de procesos a utilizar, y el nombre del ejecutable.

# Grupo en el que se va a ejecutar 
#SBATCH --partition=C1Mitad1

# Nombre del trabajo. Puede cambiarlo
# para identificarlo de forma más sencilla
#SBATCH --job-name=zairaMartinez

#Número máximo de tareas asignadas por nodo.
#Valor recomendado: 12 para C1, 24 para C2, 24 para GPU.
#SBATCH --ntasks-per-node=6 

#Número de procesos a ejecutar de forma simultánea (en total,
#teniendo en cuenta a todos los nodos). En este ejemplo
#se usarían 6 nodos tipo C2, y en cada uno de los nodos se usarían
#24 procesos, para el total de 144.
#SBATCH --ntasks=12

# archivo de log donde quedará lo que imprima su software por pantalla.
#Lo recomendable es que el programa no imprima nada a pantalla, sino a fichero directamente 
#SBATCH --output=result-paralelo1.log

#mem especifica la cantidad máxima de memoria que podrá utilizar en cada nodo.
#El valor 0 indica que podrá utilizar toda la memoria.
#SBATCH --mem=0

#Si el proceso tarda más del tiempo especificado en time automáticamente se terminará, 
#lo que permitirá evitar que un proceso en el que hubo algún tipo de 
#error, como un bucle infinito, se quede ejecutando por mucho tiempo.
#El valor 0 indica que no se mata al proceso nunca, con lo que en caso
#de errores de ese tipo lo deberá terminar manualmente (con scancel)
#SBATCH --time=0

#SBATCH --mail-type=END,FAIL
#SBATCH --mail-user=zaira.martinez@cimat.mx

# En la línea siguiente cambie la ruta, el nombre del ejecutable MPI
# (debe compilar con mpic++.openmpi, mpicc.openmpi o mpifort.openmpi) e incluya
# al final los parametros de su ejecutable si procede
mpirun.openmpi -np ${SLURM_NTASKS} /home/est_posgrado_zaira.martinez/computoEv/tarea1

# Para enviarlo en la bash escriba sbatch nombreDeEsteFichero