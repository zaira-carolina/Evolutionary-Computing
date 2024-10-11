Para ejecutar el programa en el clúster:

    mpic++ -O2 tarea1.cpp -o tarea1
    sbatch script.sh

La imagen se va a guardar en un archivo:
    
    "imagenParalelo-1024.pgm"

Debe de tardar máximo 4 minutos. Además, se va a guardar otro archivo donde tiene el tiempo en total:

    result-paralelo1.log


Para cambiar el nombre: línea 45. 
Para cambiar el tamaño: línea 13 y línea 14. 

Algunas fuentes extras: 

https://stackoverflow.com/questions/37237996/mpi-load-balancing-algorithm-master-slave-model
https://github.com/EStog/mandelbrotc-/blob/master/code/mandelbrot_mpi_send_recv/main.cpp
https://mpitutorial.com/tutorials/dynamic-receiving-with-mpi-probe-and-mpi-status/
https://stackoverflow.com/questions/58396548/mpi-send-and-receive-after-task-is-completed-is-it-even-possible


