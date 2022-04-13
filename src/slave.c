#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "utilities.h"

#define MAX_LENGTH 256
#define MAX_ID_LENGTH 20
#define COMMAND  "minisat %s | grep -o -e \"Number of.*[0-9]\\+\" -e \"CPU time.*s\" -e \".*SATISFIABLE\" | tr -s \" \" | tr \"\n\" \"\t\""


void get_minisat_output(char * minisat_buf, char * file_name);

void write_to_stdout(char * result_buf, int length);

int append_other_data(char * result_buf, char * file_buf, char * minisat_buf);



int main(int argc, char *argv[])
{

    // problema: para que el read termine, hay que hacer close del fd desde la app, pero cuando hacemos eso no podemos asignar nuevas tareas. Vamos a usar getline

    char *file_name = NULL; // al ser null, la funcion nos reserva la memoria que luego tenemos que liberar
    size_t l = 0;
    ssize_t count;
    char result_buf[MAX_LENGTH] = {0};

    while ((count = getline(&file_name, &l, stdin)) > 0)
    {
        file_name[count - 1] = 0;

        char minisat_buf[MAX_LENGTH] = {0};
        
        get_minisat_output(minisat_buf, file_name);

        int length = append_other_data(result_buf, file_name, minisat_buf);

        write_to_stdout(result_buf, length);

    }

    free(file_name);
    return 0;

    
}

int append_other_data(char * result_buf, char * file_buf, char * minisat_buf) {
    return snprintf(result_buf, MAX_LENGTH, "File: %s \t Process: %d \t %s\n", file_buf, getpid(), minisat_buf);
}

void get_minisat_output(char * minisat_buf, char * file_name) {

        char cmd_array[MAX_LENGTH] = {0};

        sprintf(cmd_array, COMMAND, file_name);

        FILE * result_file = popen(cmd_array, "r");

        if (result_file) {
            fread(minisat_buf, 1, MAX_LENGTH, result_file);
            fclose(result_file);
        }
}

void write_to_stdout(char * result_buf, int length) {
    int r;
        if ((r = write(STDOUT_FILENO, &length, sizeof(int))) == -1)
        {
            error_exit("Error writing\n", WRITE_ERROR);
        }

        if ((r = write(STDOUT_FILENO, result_buf, sizeof(char) * (length))) == -1)
        {
            error_exit("Error writing\n", WRITE_ERROR);
        }
}
