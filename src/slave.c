#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "utilities.h"

#define MAX_LENGTH 256
#define MAX_ID_LENGTH 20

void print_result(char *file_name, FILE *opStream);

int main(int argc, char *argv[])
{

    // problema: para que el read termine, hay que hacer close del fd desde la app, pero cuando hacemos eso no podemos asignar
    // nuevas tareas. Vamos a usar getline

    char *buf = NULL; // al ser null, la funcion nos reserva la memoria que luego tenemos que liberar
    size_t length = 0;
    ssize_t count;

    while ((count = getline(&buf, &length, stdin)) > 0)
    {
        buf[count - 1] = 0;
        int len = (int) count;
        int r;
        if ((r = write(STDOUT_FILENO, &len, sizeof(int))) == -1)
        {
            error_exit("Error writing\n", WRITE_ERROR);
        }

        if ((r = write(STDOUT_FILENO, buf, sizeof(char) * (len))) == -1)
        {
            error_exit("Error writing\n", WRITE_ERROR);
        }

    }

    free(buf);
    return 0;

    
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