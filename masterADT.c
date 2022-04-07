#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <masterADT.h>

typedef struct masterCDT {
    int slave_count;
    int m_write_pipes[SLAVE_COUNT][2];
    int m_read_pipes[SLAVE_COUNT][2];
    int slave_processed[SLAVE_COUNT];

    int total_tasks;
    int current_task;
    char * files[];

    // shmem_ADT sh_mem;
    // int sh_id;

}

masterADT new_master(const char *files[], int total_tasks, int shm_id) {
  masterADT master = calloc(1, sizeof(masterCDT));

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
        if (pipe(master->m_read_pipes[i]) == -1) { printf("Error with creating pipe\n"); return 1; }
        if (pipe(master->m_write_pipes[i]) == -1) { printf("Error with creating pipe\n"); return 1; }
    }

    for (i = 0; i < master->slave_count; i++)
    {
        pids[i] = fork();

        if (pids[i] == -1) { printf("Error with creating process\n"); exit(2); }

        if (pids[i] == 0)
        {
            // Child

            int childReadfd = appWritePipes[i][0];
            int childWritefd = appReadPipes[i][1];

            int j;
            for (j = 0; j< PROCESSES; j++) {
                if (j == i) {
                    close(appWritePipes[j][1]);
                    close(appReadPipes[j][0]);
                }
                else {
                    close(appWritePipes[j][0]);
                    close(appWritePipes[j][1]);
                    close(appReadPipes[j][0]);
                    close(appReadPipes[j][1]);
                }

            }
            

            dup2(childReadfd, STDIN_FILENO);
            dup2(childWritefd, STDOUT_FILENO);

            close(childWritefd);
            close(childReadfd);

            char *args[] = {"./child", NULL};

            if (execvp(args[0], args) == -1)
            {
                perror("Error al ejecutar programa hijo");
            }
        }

        else {
            // Parent
            int j;
            for (j = 0; j < PROCESSES; j++)
            {
                close(appWritePipes[j][0]);
                close(appReadPipes[j][1]);
            }

        }
    }
}

void clean_resources(masterADT master){
    int i;
    for (i = 0; i < master->slave_count; i++) {
        close(master->write_pipes[i][0]);
        close(master->read_pipes[i][1]);
    }

    // clean sh_mem
}


void free_master(masterADT master) {
  free(master);
  // free shared mem
}

void test_send(masterADT master) {
        for (i = 0 ; i < master->slave_count ; i++) {
        char * s = "Holaaaa\n";
        int len = strlen(s);
        if (write(master->write_pipes[i][1], &len, sizeof(int)) == -1) {
            perror("Error at writing\n");
            exit(2);
        }
        if (write(amaster->write_pipes[i][1], s, sizeof(char)*(len+1)) == -1) {
            perror("Error at writing\n");
            exit(2);
        }
        close(write_pipes[i][1]);
    }
}

void test_read(masterADT master) {
        for (i = 0 ; i < master->slave_count ; i++) {
        int length;
        char s[100] = {0};
        read(master->read_pipes[i][0], &length, sizeof(int));
        read(master->read_pipes[i][0], s, sizeof(char)*(length + 1));
        printf(s);
    }
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