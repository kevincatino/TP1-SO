#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "masterADT.h"

#define READ 0700
#define WRITE 0007

#define RESULT_PATH "./results.txt"


int main(int argc, char *argv[])
{
    
    masterADT master = new_master(argv + 1, argc - 1, "./results.txt");
    init_slaves(master);
    assign_initial_tasks(master); //2 tareas iniciales por esclavo
    fetch_and_assign_new_tasks(master); //si se libera alguno, le damos una tarea nueva
    // free_master(master);

}


        
   