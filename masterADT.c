#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "masterADT.h"
#include <sys/wait.h>

#define MAX_SLAVE_QTY 5
#define FILES_PER_SLAVE 2
#define MAX_OUT_LEN 256
#define RESULT_PATH "./results.txt"

typedef struct masterCDT {
    int slave_count;
    int pids[MAX_SLAVE_QTY];
    int write_pipes[MAX_SLAVE_QTY][2];
    int read_pipes[MAX_SLAVE_QTY][2];
    int slave_processed[MAX_SLAVE_QTY];

    int total_tasks;
    int current_task;
    char ** files;

    // shmem_ADT sh_mem;
    // int sh_id;

} masterCDT;

masterADT new_master(char **files, int total_tasks, int shm_id) {
    masterADT master = calloc(1, sizeof(masterCDT));
    master->slave_count = MAX_SLAVE_QTY;

    if(master == NULL)
    // error

    master->total_tasks = total_tasks;
    master->files = files;

  // newMaster->sh_mem = otro ADT
  // newMaster->sh_mem_id = id del shared memory;

    return master;
}

void init_slaves(masterADT master) {

    int i;
    for (i = 0; i < master->slave_count; i++)
    {
        if (pipe(master->read_pipes[i]) == -1) { printf("Error with creating pipe\n"); exit(1); }
        if (pipe(master->write_pipes[i]) == -1) { printf("Error with creating pipe\n"); exit(1); }
    }

    for (i = 0; i < master->slave_count; i++)
    {
        master->pids[i] = fork();

        if (master->pids[i] == -1) { printf("Error with creating process\n"); exit(2); }

        if (master->pids[i] == 0)
        {
            // Child

            int childReadfd = master->write_pipes[i][0];
            int childWritefd = master->read_pipes[i][1];

            int j;
            for (j = 0; j < master->slave_count; j++) {
                if (j == i) {
                    close(master->write_pipes[j][1]);
                    close(master->read_pipes[j][0]);
                }
                else {
                    close(master->write_pipes[j][0]);
                    close(master->write_pipes[j][1]);
                    close(master->read_pipes[j][0]);
                    close(master->read_pipes[j][1]);
                }

            }
            

            dup2(childReadfd, STDIN_FILENO);
            dup2(childWritefd, STDOUT_FILENO);

            close(childReadfd);
            close(childWritefd);

            char *args[] = {"./child", NULL};

            if (execvp(args[0], args) == -1)
            {
                perror("Error al ejecutar programa hijo");
            }
        }
    }

        // Parent
    int j;
    for (j = 0; j < master->slave_count ; j++)
    {
        close(master->write_pipes[j][0]);
        close(master->read_pipes[j][1]);
    }
}


void free_master(masterADT master) {
    int i;
    for (i = 0; i < master->slave_count; i++)
    {
        wait(NULL);
    }
    free(master);
  // free shared mem
}

void test_send(masterADT master) {
    int i;
    for (i = 0 ; i < master->slave_count ; i++) {
        char * s = "Holaaaa\n";
        int len = strlen(s);
        if (write(master->write_pipes[i][1], &len, sizeof(int)) == -1) {
            perror("Error at writing\n");
            exit(2);
        }
        if (write(master->write_pipes[i][1], s, sizeof(char)*(len+1)) == -1) {
            perror("Error at writing\n");
            exit(2);
        }
        close(master->write_pipes[i][1]);
    }
}

void test_read(masterADT master) {
    int i;
    for (i = 0 ; i < master->slave_count ; i++) {
        int length;
        char s[100] = {0};
        read(master->read_pipes[i][0], &length, sizeof(int));
        read(master->read_pipes[i][0], s, sizeof(char)*(length + 1));
        printf(s);
    }
    for (i = 0; i < master->slave_count; i++)
    {
        wait(NULL);
    }
    return;
}

// int main(int argc, char *argv[])
// {

//     int i;

//     for (i = 0; i < PROCESSES; i++)
//     {
//         if (pipe(appReadPipes[i]) == -1) { printf("Error with creating pipe\n"); return 1; }
//         if (pipe(appWritePipes[i]) == -1) { printf("Error with creating pipe\n"); return 1; }
//     }

//     for (i = 0; i < PROCESSES; i++)
//     {
//         pids[i] = fork();

//         if (pids[i] == -1) { printf("Error with creating process\n"); return 2; }

//         if (pids[i] == 0)
//         {
//             // Child

//             int childReadfd = appWritePipes[i][0];
//             int childWritefd = appReadPipes[i][1];

//             int j;
//             for (j = 0; j< PROCESSES; j++) {
//                 if (j == i) {
//                     close(appWritePipes[j][1]);
//                     close(appReadPipes[j][0]);
//                 }
//                 else {
//                     close(appWritePipes[j][0]);
//                     close(appWritePipes[j][1]);
//                     close(appReadPipes[j][0]);
//                     close(appReadPipes[j][1]);
//                 }

//             }
            

//             dup2(childReadfd, STDIN_FILENO);
//             dup2(childWritefd, STDOUT_FILENO);

//             close(childWritefd);
//             close(childReadfd);

//             char *args[] = {"./child", NULL};

//             if (execvp(args[0], args) == -1)
//             {
//                 perror("Error al ejecutar programa hijo");
//             }
//         }
//     }

//     // Parent

//     int j;
//     for (j = 0; j < PROCESSES; j++)
//     {
//         close(appWritePipes[j][0]);
//         close(appReadPipes[j][1]);
//     }

//     fd_set rfds;

//     for (i = 0 ; i < PROCESSES; i++)
//         FD_SET(appReadPipes[i][0], &rfds);

//     int tasksPending = 1;
//     while(tasksPending) {
//         FD_ZERO(&rfds);
//         select(appReadPipes[PROCESSES-1][1]+1, &rfds, NULL, NULL, NULL);

//         int i;
//         for (i = 0 ; i < PROCESSES ; i++) {
//             if (FD_ISSET(appReadPipes[i][0], &rfds)) {
//                 // read ()
//                 // postear en shm
//                 // si hay mas tareas -> write(pipe[1]), else -> close(pipe[1]); tasksPending=0; break;
//             }
//         }

//         // cierro todos los pipes restantes
//     }


        
//     // }
//     for (i = 0 ; i < PROCESSES ; i++) {
//         char * s = "Holaaaa\n";
//         int len = strlen(s);
//         if (write(appWritePipes[i][1], &len, sizeof(int)) == -1) {
//             perror("Error at writing\n");
//             exit(2);
//         }
//         if (write(appWritePipes[i][1], s, sizeof(char)*(len+1)) == -1) {
//             perror("Error at writing\n");
//             exit(2);
//         }
//         close(appWritePipes[i][1]);
//     }

//     for (i = 0 ; i < PROCESSES ; i++) {
//         int length;
//         char s[100] = {0};
//         read(appReadPipes[i][0], &length, sizeof(int));
//         read(appReadPipes[i][0], s, sizeof(char)*(length + 1));
//         printf(s);
//     }

//     for (i = 0; i < PROCESSES; i++)
//     {
//         wait(NULL);
//     }
//     return 0;
// }