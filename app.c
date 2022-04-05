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
    int writePipes[PROCESSES][2];
    int readPipes[PROCESSES][2];
    int i;

    for (i = 0; i < PROCESSES + 1; i++)
    {
        if (pipe(writePipes[i]) == -1)
        {
            printf("Error with creating pipe\n");
            return 1;
        }
        if (pipe(readPipes[i]) == -1)
        {
            printf("Error with creating pipe\n");
            return 1;
        }
    }

    for (i = 0; i < PROCESSES; i++)
    {
        pids[i] = fork();
        if (pids[i] == -1)
        {
            printf("Error with creating process\n");
            return 2;
        }

        if (pids[i] == 0)
        {
            // Child

        
            int readfd = writePipes[i][0];
            int writefd = readPipes[i][1];

            int j;
            for (j = 0; j< PROCESSES; j++) {
                if (j != i) {
                    close(writePipes[j][0]);
                    close(writePipes[j][1]);
                }
            }
            

            dup2(readfd, STDIN_FILENO);
            dup2(writefd, STDOUT_FILENO);

            close(writefd);
            close(readfd);

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
        close(writePipes[j][0]);
        close(readPipes[j][1]);
    }

    // while(1) {
    // // select()
    //     // si p1 esta ready:
	// // read(p1)
	// // postear en shm
	// // si hay mas tareas:
	// 	// write(p2)
	// // else:
	// 	// close(p2)

        
    // }
    for (i = 0 ; i < PROCESSES ; i++) {
        char * s = "Holaaaa\n";
        int len = strlen(s);
        if (write(writePipes[i][1], &len, sizeof(int)) == -1) {
            perror("Error at writing\n");
            exit(2);
        }
        if (write(writePipes[i][1], s, sizeof(char)*(len+1)) == -1) {
            perror("Error at writing\n");
            exit(2);
        }
    }

    for (i = 0 ; i < PROCESSES ; i++) {
        int length;
        char s[100] = {0};
        read(readPipes[i][0], &length, sizeof(int));
        read(readPipes[i][0], s, sizeof(char)*(length + 1));
        printf(s);
    }

    for (i = 0; i < PROCESSES; i++)
    {
        wait(NULL);
    }
    return 0;
}