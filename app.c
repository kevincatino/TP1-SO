#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROCESSES 4

typedef struct {
    char name[80];
    int number;
} data;

int main(int argc, char* argv[]) {
    int pids[PROCESSES];
    int writePipes[PROCESSES + 1][2];
    int readPipes[PROCESSES + 1][2];
    int i;
    for (i = 0; i < PROCESSES + 1; i++) {
        if (pipe(writePipes[i]) == -1) { printf("Error with creating pipe\n"); return 1; }
        if (pipe(readPipes[i]) == -1) { printf("Error with creating pipe\n"); return 1; }
    }

    for (i = 0; i < PROCESSES; i++) {
        pids[i] = fork();
        if (pids[i] == -1) { printf("Error with creating process\n"); return 2; }

        if (pids[i] == 0) {
            // Child

        int readfd = writePipes[i][0];
        int writefd = readPipes[i][1];

        dup2(readfd, STDIN);

        char *args[]={"./child"};

        if(execvp(args[0],args) == -1) {
            perror("Error al ejecutar programa hijo");
        }

        }
    }

    // Parent

    int j;
    for (j = 0; j < PROCESSES + 1; j++) {
        if (j != PROCESSES) {
            close(writePipes[j][0]);
            close(readPipes[j][1]);
        }
    }

    for (i = 0; i < PROCESSES; i++) {
        int a = i;
        if (write(writePipes[i][1],&a, sizeof(int)) == -1) { printf("Error at writing\n"); return 4; }
    }

    for (i = 0; i < PROCESSES; i++) {
        int a;
        if (read(readPipes[i][0], &a, sizeof(int)) == -1) { printf("Error at reading\n"); return 3; }
        printf("Recibi %d en main\n", a);
    }

    for (i = 0; i < PROCESSES; i++) {
        wait(NULL);
    }
    return 0;
}