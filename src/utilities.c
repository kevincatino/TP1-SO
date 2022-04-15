// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "utilities.h"



void error_exit(char * msg, 
int code) {
    perror(msg);
    exit(code);
}