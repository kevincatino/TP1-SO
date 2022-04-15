// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

    char *file_name = NULL; 
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
    int length = snprintf(result_buf, MAX_LENGTH, "File: %s \t Process: %d \t %s\n", file_buf, getpid(), minisat_buf);

    if (length < 0)
        error_exit("Error printing to string", WRITE_ERROR);

    return length;
}

void get_minisat_output(char * minisat_buf, char * file_name) {

        char cmd_array[MAX_LENGTH] = {0};

        if (sprintf(cmd_array, COMMAND, file_name) < 0)
            error_exit("Error printing to string", WRITE_ERROR);

        FILE * result_file = popen(cmd_array, "r");

        if (result_file == NULL)
            error_exit("Error opening minisat process", POPEN_ERROR);

        if (result_file) {
            fread(minisat_buf, 1, MAX_LENGTH, result_file);
            if (fclose(result_file) == EOF)
                error_exit("Error closing file", FILE_ERROR);
        }
}

void write_to_stdout(char * result_buf, int length) {
        if (write(STDOUT_FILENO, &length, sizeof(int)) == -1)
        {
            error_exit("Error writing\n", WRITE_ERROR);
        }

        if (write(STDOUT_FILENO, result_buf, sizeof(char) * (length)) == -1)
        {
            error_exit("Error writing\n", WRITE_ERROR);
        }
}
