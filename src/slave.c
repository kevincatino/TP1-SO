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
#define COMMAND  "minisat %s | grep -o -e \"Number of.*[0-9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\" | tr \"\\n\" \"\\t\" | tr \" \" \"\\t\" | tr -d \"\\t\""


void get_minisat_output(char * minisat_buf, char * file_name);

void write_to_stdout(char * result_buf, int length);

int form_final_output(char * result_buf, char * file_buf, int variables, int clauses, double cpu_time, char * satisfiability);



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

        char satisfiability[MAX_LENGTH] = {0};
        int variables, clauses;
        double cpu_time;

        sscanf(minisat_buf, "Numberofvariables:%dNumberofclauses:%dCPUtime:%lfs%s", &variables, &clauses, &cpu_time, satisfiability);

        int length = form_final_output(result_buf, file_name, variables, clauses, cpu_time, satisfiability);

        write_to_stdout(result_buf, length);

    }


    free(file_name);
    return 0;

    
}

int form_final_output(char * result_buf, char * file_buf, int variables, int clauses, double cpu_time, char * satisfiability) {
    int length = snprintf(result_buf, MAX_LENGTH, "File: %-30s\t Process: %-5d\t Variables: %-5d\t Clauses: %-5d\t CPU time: %-8.5lfs\t %-11s\n", file_buf, getpid(), variables, clauses, cpu_time, satisfiability);

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
