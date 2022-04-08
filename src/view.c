#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define STDIN 0
#define MAX_ID_LENGTH 20
#define MAX_OUTPUT_LENGTH 256

int main(int argc, char* argv[]) {
    if (argc == 1) {
        // recibo de STDIN
        return 0;
    }

    // recibo de shared memory
    char * shared = argv[1];
    
    // imprimir por salida estandar
    
}


// int get_id_stdin() {
//     char buffer[MAX_ID_LENGTH + 1];
//     int count_read = read(STDIN, buffer, MAX_ID_LENGTH);
//     if(count_read == -1) {
//         perror("Error reading");
//         exit(EXIT_FAILURE); // Exit failure esta definida en alguna de las librerias creo
//     }
//     buffer[count] = 0;      // El 0 del final
//     return atoi(buffer);    // ? Pasamos a numero el buffer y lo usamos como ID
// }