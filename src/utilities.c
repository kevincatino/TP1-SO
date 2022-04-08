#include "utilities.h"



void error_exit(char * msg, 
int code) {
    perror(msg);
    exit(code);
}