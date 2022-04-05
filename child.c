#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char path[100] = {0};
int length;

int main(int argc, char * argv[]) {

    // if (read(STDIN_FILENO, &y, sizeof(int)) == -1) { printf("Error at reading\n"); return 3; }
    // if (write(STDOUT_FILENO,&y, sizeof(int)) == -1) { printf("Error at writing\n"); return 4; }

        // Del lado del worker
    int r;
    if((r = read(STDIN_FILENO, &length, sizeof(int)) == -1)) { perror("Error at reading\n"); return 5; }

    if ((r = read(STDIN_FILENO, path, sizeof(char)*(length + 1))) == -1) { perror("Error at reading\n"); return 5; }

    if ((r = write(STDOUT_FILENO, &length, sizeof(int))) == -1) { perror("Error at writing\n"); return 5; }

    if ((r = write(STDOUT_FILENO, path, sizeof(char)*(length + 1))) == -1) { perror("Error at writing\n"); return 5; }

    printf(path);


    // if (r == 0) {
    //     // no hay mas trabajo
    //     close(STDIN_FILENO);
    //     exit(0);
    // }

    // else { perror("read"); exit(2); }
        

    return 0;
}