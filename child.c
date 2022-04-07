#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define MAX_LENGTH 256 
#define MAX_ID_LENGTH 20


char path[100] = {0}; 
int length;


void print_result(char * file_name, FILE * opStream);

int main(int argc, char * argv[]) {

    int r;

    while((r = read(STDIN_FILENO, &length, sizeof(int)) > 0)) {
        if ((r = read(STDIN_FILENO, path, sizeof(char)*(length + 1))) == -1) { perror("Error at reading\n"); exit(5); }
    }

    if (r == 0) {
        // no hay mas trabajo
        close(STDIN_FILENO);
        exit(0);
    }

    else { perror("read"); exit(2); }

    if (r == -1) { perror("Error at reading\n"); exit(4); }

    // termina el proceso de los archivos. No es necesario cerrar STDIN no?
    
    // procesar la data

    if ((r = write(STDOUT_FILENO, &length, sizeof(int))) == -1) { perror("Error at writing\n"); return 5; }

    if ((r = write(STDOUT_FILENO, path, sizeof(char)*(length + 1))) == -1) { perror("Error at writing\n"); return 5; }


}

// void print_result(char * file_name, FILE * opStream) {
//     char op[MAX_LENGTH + 1]; // output

//     int total = fread(op, sizeof(char), MAX_LENGTH, opStream); // lle MAX_LENGTH items of data, sizeof de longitud, desde el opStream (el output), y las guarda en output
//     op[total] = 0;

//     int file_name_length = strlen(file_name);

//     strcat(op, "File ");
//     strcat(op, file_name);

//     char slave_ID_string[MAX_ID_LENGTH];

//     pid_t slave_ID = getpid(); // por ahora lo dejo como struct, total es como un int

//     total += snprintf(slave_ID_string, 20, "Slave ID: %d\n, slave_ID", slave_ID) + file_name_length + 7; //snprintf escriben at the most size (20) bytes. Analizar el porq del +7

//     strcat(op, slave_ID_string);
//     op[total] = 0;

//     write(STDOUT_FILENO, op, total);
//  }