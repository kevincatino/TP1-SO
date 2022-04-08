#define PIPE_ERROR 1
#define FILE_ERROR 2
#define FORK_ERROR 3
#define EXEC_ERROR 4
#define SELECT_ERROR 5
#define WRITE_ERROR 6
#define READ_ERROR 7
#define SEMAPHORE_ERROR 8
#define SHARED_MEM_ERROR 9
#define MALLOC_ERROR 10
#define MEMORY_ERROR 11

#define FALSE 0
#define TRUE 1

#include <stdlib.h>
#include <stdio.h>

void error_exit(char * msg, int code);