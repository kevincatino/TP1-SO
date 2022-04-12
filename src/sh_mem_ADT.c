
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
#define SEM_NAME "namedSem"

//A named semaphore is identified by a name of the form
              // "/somename"

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

// si el flag es READ, se toma la key del puntero para conectarse a la memoria ya creada.
// si el flag es WRITE, se genera una nueva key junto a la memoria, y se escribe en el puntero key.
sh_mem_ADT new_sh_mem(int * key, int flag)
{
    sh_mem_ADT sh_mem_handler = calloc(1, sizeof(sh_mem_CDT)); // guarda memoria para 1 elem de sizeof size, usamos calloc para setear la memory en cero

    sh_mem_handler->flag = flag;

    if (sh_mem_handler == NULL)
        error_exit("Error allocating memory", MALLOC_ERROR);

    // usamos O_CREAT,  si el file existe no hace nada, sino lo crea y lo ponemso en cero al semaphore para q este bloqueado
    sh_mem_handler->semaphore = sem_open(SEM_NAME, O_CREAT, S_IRUSR | S_IWUSR, 0); // usamos -> y no . porq sh_mem_handler es un ADT y apunta a un elemento (semaphore) del CDT

    if (sh_mem_handler->semaphore == SEM_FAILED) // on error sem_open returns sem_failes ESTA EN MAN
        error_exit("Error opening semaphore", SHARED_MEM_ERROR);

    if (flag & WRITE)
    {
        sh_mem_handler->id = shmget(IPC_PRIVATE, sizeof(struct sh_mem_t), IPC_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH); // si no le pasamos IPC_CREAT busca un segmento asociado a la key, sino lo crea. IPC_EXCL garantiza que se crea un nuevo segmento. key en este caso es redundante ya que la shared memory es nueva

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
    // si la funcion viene de view, propongo que la key sea el id (de parametro). Si es de escritura, que la key sea de una memoria a la cual voy a querer ir a buscar (para leerla)
    
    sh_mem_handler->sh_mem = (sh_mem_t *)shmat(id, NULL, 0); // In order for a process to access that previously created shared memory, it will be necessary for some process variable to "point" to that memory area that does not belong to its address space. To do this, this system call is used, which allows linking that memory area to the logical address of the process.

    // If shmaddr is a null pointer, the segment is attached at the first available address as selected by the system.

    // Si los flags estan en cero, se puede leer y escribir en la memoria compartida
    if (sh_mem_handler->sh_mem == (void *)-1)
        error_exit("Error getting pointer to shared memory", SHARED_MEM_ERROR);

    if (sh_mem_handler->flag & READ) // master escribe y la vista lee, por eso en vista esta en cero y master en 1 VER B
        sh_mem_handler->sh_mem->curr_reading_line = 0;
    else
    {
        sh_mem_handler->sh_mem->curr_writing_line = 0;
    }
}

void write_sh_mem(sh_mem_ADT sh_mem_handler, const char *msg)
{
    if (sh_mem_handler->sh_mem->curr_writing_line == MAX_LINES) // llegue al tope de lo q puedo escribir
    {
        error_exit("Out of memory", MEMORY_ERROR);
    }

    snprintf(sh_mem_handler->sh_mem->buff[sh_mem_handler->sh_mem->curr_writing_line++], MAX_BUF_SIZE, "%s", msg); // escribo msg en el sh_m, con max_buff_size de tamanio y %s de formato

    if (sem_post(sh_mem_handler->semaphore) == -1) // le digo a view que ahora puede leer
        error_exit("Error writing in semaphore", SEMAPHORE_ERROR);

}

void read_sh_mem(sh_mem_ADT sh_mem_handler, char *buff)
{
    if (sem_wait(sh_mem_handler->semaphore) == -1) // espero a que master me permita leer
        error_exit("Error writing in semaphore", SEMAPHORE_ERROR);

    snprintf(buff, MAX_BUF_SIZE, "%s", sh_mem_handler->sh_mem->buff[sh_mem_handler->sh_mem->curr_reading_line++]); // redirecciona la salida del printf al buffer (en este caso lo que este en %s)
}

void free_sh_mem(sh_mem_ADT sh_mem_handler)
{
    if (((sh_mem_handler->flag) & READ) && (sem_close(sh_mem_handler->semaphore) == -1))
        error_exit("Error closing semaphore", SEMAPHORE_ERROR);

    if (((sh_mem_handler->flag) & READ) && (shmdt(sh_mem_handler->sh_mem) == -1))
        error_exit("Error detaching shared memory", SHARED_MEM_ERROR);

    if (((sh_mem_handler->flag) & READ) && (shmctl(sh_mem_handler->id, IPC_RMID, NULL) == -1))
        error_exit("Error in destroying shared memory", SHARED_MEM_ERROR);

    if (((sh_mem_handler->flag) & READ) && (sem_unlink(SEM_NAME) == -1)) // remueve el nombre del semaforo
        error_exit("Error unlinking name from semaphore", SEMAPHORE_ERROR);
    free(sh_mem_handler);
}