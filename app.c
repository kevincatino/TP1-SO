#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "masterADT.h"

#define RESULT_PATH "./results.txt"


int main(int argc, char *argv[])
{
    
    masterADT master = new_master(argv + 1, argc - 1, ".");
    init_slaves(master);
    assign_initial_tasks(master);
    fetch_and_assign_new_tasks(master);
    free_master(master);

}


        
   