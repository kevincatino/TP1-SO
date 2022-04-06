#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROCESSES 4

typedef struct
{
    char name[80];
    int number;
} data;

int main(int argc, char *argv[])
{
    int pids[PROCESSES];
    int appWritePipes[PROCESSES][2];
    int appReadPipes[PROCESSES][2];
    int i;

    for (i = 0; i < PROCESSES + 1; i++)
    {
        if (pipe(appReadPipes[i]) == -1) { printf("Error with creating pipe\n"); return 1; }
        if (pipe(appWritePipes[i]) == -1) { printf("Error with creating pipe\n"); return 1; }
    }

    for (i = 0; i < PROCESSES; i++)
    {
        pids[i] = fork();

        if (pids[i] == -1) { printf("Error with creating process\n"); return 2; }

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
    }

    // Parent

    int j;
    for (j = 0; j < PROCESSES; j++)
    {
        close(appWritePipes[j][0]);
        close(appReadPipes[j][1]);
    }

    fd_set rfds;

    for (i = 0 ; i < PROCESSES; i++)
        FD_SET(appReadPipes[i][0], &rfds);

    int tasksPending = 1;
    while(tasksPending) {
        FD_ZERO(&rfds);
        select(appReadPipes[PROCESSES-1][1]+1, &rfds, NULL, NULL, NULL);

        int i;
        for (i = 0 ; i < PROCESSES ; i++) {
            if (FD_ISSET(appReadPipes[i][0], &rfds)) {
                // read ()
                // postear en shm
                // si hay mas tareas -> write(pipe[1]), else -> close(pipe[1]); tasksPending=0; break;
            }
        }

        // cierro todos los pipes restantes
    }


        
    // }
    for (i = 0 ; i < PROCESSES ; i++) {
        char * s = "Holaaaa\n";
        int len = strlen(s);
        if (write(appWritePipes[i][1], &len, sizeof(int)) == -1) {
            perror("Error at writing\n");
            exit(2);
        }
        if (write(appWritePipes[i][1], s, sizeof(char)*(len+1)) == -1) {
            perror("Error at writing\n");
            exit(2);
        }
        close(appWritePipes[i][1]);
    }

    for (i = 0 ; i < PROCESSES ; i++) {
        int length;
        char s[100] = {0};
        read(appReadPipes[i][0], &length, sizeof(int));
        read(appReadPipes[i][0], s, sizeof(char)*(length + 1));
        printf(s);
    }

    for (i = 0; i < PROCESSES; i++)
    {
        wait(NULL);
    }
    return 0;
}