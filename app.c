#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "masterADT.h"

int main(int argc, char *argv[])
{
    

    // Parent
    masterADT master = new_master(NULL, 10, 0);
    init_slaves(master);
    test_send(master);
    test_read(master);
    free_master(master);


    // fd_set rfds;

    // for (i = 0 ; i < PROCESSES; i++)
    //     FD_SET(appReadPipes[i][0], &rfds);

    // int tasksPending = 1;
    // while(tasksPending) {
    //     FD_ZERO(&rfds);
    //     select(appReadPipes[PROCESSES-1][1]+1, &rfds, NULL, NULL, NULL);

    //     int i;
    //     for (i = 0 ; i < PROCESSES ; i++) {
    //         if (FD_ISSET(appReadPipes[i][0], &rfds)) {
    //             // read ()
    //             // postear en shm
    //             // si hay mas tareas -> write(pipe[1]), else -> close(pipe[1]); tasksPending=0; break;
    //         }
    //     }

    //     // cierro todos los pipes restantes
    return 0;
    }


        
   