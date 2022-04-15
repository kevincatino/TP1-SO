// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utilities.h"
#include "./include/sh_mem_ADT.h"

#define MAX_LINES 100
#define MAX_BUF_SIZE 256
#define SEM_NAME "/namedSem" //Un named semaphore se identifica por un nombre de la forma "/algunnombre"


static void attach_to(sh_mem_ADT sh_mem_handler, int id);

typedef struct sh_mem_t
{
    int curr_writing_line;
    int curr_reading_line;
    char buff[MAX_LINES][MAX_BUF_SIZE];
} sh_mem_t;

typedef struct sh_mem_CDT
{
    sh_mem_t *sh_mem;
    int id;
    int flag;
    sem_t *semaphore;
} sh_mem_CDT;

// Si el flag es READ, se toma la key del puntero para conectarse a la memoria ya creada.
// Si el flag es WRITE, se genera una nueva key junto a la memoria, y se escribe en el puntero key.
sh_mem_ADT new_sh_mem(int * key, int flag)
{
    sh_mem_ADT sh_mem_handler = calloc(1, sizeof(sh_mem_CDT)); 

    if (sh_mem_handler == NULL)
        error_exit("Error allocating memory",  MALLOC_ERROR);

    sh_mem_handler->flag = flag;

    sh_mem_handler->semaphore = sem_open(SEM_NAME, O_CREAT, S_IRUSR | S_IWUSR, 0); 

    if (sh_mem_handler->semaphore == SEM_FAILED) 
        error_exit("Error opening semaphore", SHARED_MEM_ERROR);

    if (flag & WRITE)
    {
        sh_mem_handler->id = shmget(IPC_PRIVATE, sizeof(struct sh_mem_t), IPC_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH); 

        if (sh_mem_handler->id == -1)
            error_exit("Error allocating shared memory", SHARED_MEM_ERROR);

        (*key) = sh_mem_handler->id;

        
    }  

    else if (flag & READ) {
        sh_mem_handler->id = (*key);
    }

    attach_to(sh_mem_handler, sh_mem_handler->id);

    return sh_mem_handler;
}

static void attach_to(sh_mem_ADT sh_mem_handler, int id)
{
    sh_mem_handler->sh_mem = (sh_mem_t *)shmat(id, NULL, 0); 

    if (sh_mem_handler->sh_mem == (void *)-1)
        error_exit("Error getting pointer to shared memory", SHARED_MEM_ERROR);

    if (sh_mem_handler->flag & READ) // La vista lee por ende usamos el flag READ, en master se utiliza WRITE 
        sh_mem_handler->sh_mem->curr_reading_line = 0;
    else
    {
        sh_mem_handler->sh_mem->curr_writing_line = 0;
    }
}

void write_sh_mem(sh_mem_ADT sh_mem_handler, const char *msg)
{
    if (sh_mem_handler->sh_mem->curr_writing_line == MAX_LINES) // Se llego al tope de lo que se puede escribir
    {
        error_exit("Out of memory", MEMORY_ERROR);
    }

    if (snprintf(sh_mem_handler->sh_mem->buff[sh_mem_handler->sh_mem->curr_writing_line++], MAX_BUF_SIZE, "%s", msg) < 0) 
         error_exit("Error writing to shared memory", WRITE_ERROR);

    if (sem_post(sh_mem_handler->semaphore) == -1) // Si no hay errores, view ahora puede leer
        error_exit("Error writing in semaphore", SEMAPHORE_ERROR);

}

void read_sh_mem(sh_mem_ADT sh_mem_handler, char *buff)
{
    if (sem_wait(sh_mem_handler->semaphore) == -1) // Espera a que master pueda leer
        error_exit("Error writing in semaphore", SEMAPHORE_ERROR);

    if (snprintf(buff, MAX_BUF_SIZE, "%s", sh_mem_handler->sh_mem->buff[sh_mem_handler->sh_mem->curr_reading_line++]) < 0) 
        error_exit("Error reading from shared memory", READ_ERROR);
}

void free_sh_mem(sh_mem_ADT sh_mem_handler)
{
    if ((sem_close(sh_mem_handler->semaphore) == -1))
        error_exit("Error closing semaphore", SEMAPHORE_ERROR);

    if ((shmdt(sh_mem_handler->sh_mem) == -1))
        error_exit("Error detaching shared memory", SHARED_MEM_ERROR);

    // Solo destruimos la shared mem y desanclamos el nombre del semaforo desde view

    if (((sh_mem_handler->flag) & READ) && (shmctl(sh_mem_handler->id, IPC_RMID, NULL) == -1))
        error_exit("Error in destroying shared memory", SHARED_MEM_ERROR); 

    if (((sh_mem_handler->flag) & READ) && (sem_unlink(SEM_NAME) == -1))
        error_exit("Error unlinking name from semaphore", SEMAPHORE_ERROR);


    free(sh_mem_handler);
}