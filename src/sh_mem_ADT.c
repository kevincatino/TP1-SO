
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include "./sh_mem_ADT.h"

#define MAX_BUF_SIZE 256
#define MAX_LINES 30 //pongo 30 pero no se cuantas

// typedef struct shared_memory{
//     int 

// }

// sh_mem_ADT new_sh_mem() {
//   sh_mem_ADT shMemHandler = calloc(1, sizeof(shMemHandlerCDT));
//   if(shMemHandler == NULL)
//     handle_error("calloc");

//   shMemHandler->semaph = sem_open(SEM_NAME, O_CREAT, 0660, 0);
//   if (shMemHandler->semaph == SEM_FAILED)
//     handle_error("sem_open");

//   return shMemHandler;
// }