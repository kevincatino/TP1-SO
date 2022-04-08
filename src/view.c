#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utilities.h"
#include "sh_mem_ADT.h"

#define MAX_ID_LENGTH 20
#define MAX_OUTPUT_LENGTH 256

int main(int argc, char *argv[])
{
    int key;
    if (argc == 1)
    {
        // recibo de STDIN el id de shared memory
        char buff[MAX_ID_LEN + 1];
        int count = read(STDIN_FILENO, buff, MAX_ID_LENGTH);
        if (count == -1)
        {
            error_exit("Error reading from STDIN", WRITE_ERROR);
        }
        buff[count] = 0;
        key =  atoi(buff);
    }
    else
    {
        // recibo por arumento el id de shared memory
        key = atoi(argv[1]);
    }

    sh_mem_ADT sh_mem = new_sh_mem(key, READ);

    // imprimir por salida estandar
}
