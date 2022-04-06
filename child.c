#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char path[100] = {0};
int length;

int main(int argc, char * argv[]) {

    int r;

    while((r = read(STDIN_FILENO, &length, sizeof(int)) > 0)) {
        if ((r = read(STDIN_FILENO, path, sizeof(char)*(length + 1))) == -1) { perror("Error at reading\n"); exit(5); }
    }

    if (r == -1) { perror("Error at reading\n"); exit(4); }

    // termina el proceso de los archivos. No es necesario cerrar STDIN no?


    // procesar la data

    if ((r = write(STDOUT_FILENO, &length, sizeof(int))) == -1) { perror("Error at writing\n"); return 5; }

    if ((r = write(STDOUT_FILENO, path, sizeof(char)*(length + 1))) == -1) { perror("Error at writing\n"); return 5; }



    // if (r == 0) {
    //     // no hay mas trabajo
    //     close(STDIN_FILENO);
    //     exit(0);
    // }

    // else { perror("read"); exit(2); }
        

    return 0;
}