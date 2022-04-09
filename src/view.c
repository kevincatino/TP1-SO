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
    {   // llamo por el master
        // recibo de STDIN el id de shared memory
        char buff[MAX_ID_LENGTH + 1];
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
        // recibo por argumento el id de shared memory
        key = atoi(argv[1]);
    }
    printf("la llave es %d\n", key);
    sh_mem_ADT sh_mem = new_sh_mem(&key, READ);

    // imprimir por salida estandar

    char output[MAX_OUTPUT_LENGTH];

    read_sh_mem(sh_mem, output);

    int lines = atoi(output);
    int counter = 0;

    while(counter < lines) {  
        read_sh_mem(sh_mem, output);
        printf("%s\n", output);
        counter++;
    }

    free_sh_mem(sh_mem);

    return 0;
}
