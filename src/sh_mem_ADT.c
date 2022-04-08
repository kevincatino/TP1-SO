
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utilities.h"
#include "./include/sh_mem_ADT.h"

#define MAX_LINES 30 
#define MAX_BUF_SIZE 256
#define SEM_NAME "/namedSemView" 


typedef struct sh_mem_t
{
    int curr_writing_line;
    int curr_reading_line;
    bool finished_writing;
    char buff[MAX_LINES][MAX_BUF_SIZE];
} sh_mem_t;

typedef struct sh_mem_CDT
{
    sh_mem_t *sh_mem;
    int id;
    sem_t *semaphore;
} sh_mem_CDT;

sh_mem_ADT new_sh_mem(int key, int flag)
{
    sh_mem_ADT sh_mem_handler = calloc(1, sizeof(sh_mem_CDT)); // guarda memoria para 1 elem de sizeof size
    
    if (sh_mem_handler == NULL)
        exit_error("Error allocating memory", MALLOC_ERROR);

    // usamos O_CREAT,  si el file existe no hace nada, sino lo crea y lo ponemso en cero al semaphore para q este desbloqueado
    sh_mem_handler->semaphore = sem_open(SEM_NAME, O_CREAT, S_IRUSR | S_IWUSR, 0); // usamos -> y no . porq sh_mem_handler es un ADT y apunta a un elemento (semaphore) del CDT
    
    if (sh_mem_handler->semaphore == SEM_FAILED) // on error sem_open returns sem_failes ESTA EN MAN
        exit_error("Error opening semaphore", SHARED_MEM_ERROR);

    sh_mem_handler->id = shmget(key, sizeof(struct sh_mem_t), IPC_CREAT | IPC_EXCL); // si no le pasamos IPC_CREAT busca un segmento asociado a la key, sino no crea. IPC_EXCL garantiza que se crea un nuevo segmento.
    
    if (sh_mem_handler->id== -1)
        exit_error("Error allocating shared memory", SHARED_MEM_ERROR);

    return sh_mem_handler;
}


void attach_to(sh_mem_ADT sh_mem_handler, int flag) 
{
    sh_mem_handler->sh_mem = (sh_mem_t *)shmat(sh_mem_handler->id, NULL, 0); // In order for a process to access that previously created shared memory, it will be necessary for some process variable to "point" to that memory area that does not belong to its address space. To do this, this system call is used, which allows linking that memory area to the logical address of the process.
    if (sh_mem_handler->sh_mem == (void *)-1)
        exit_error("shmat");
        
    if (flag && READ) // master escribe y la vista lee, por eso en vista esta en cero y master en 1 VER B
        sh_mem_handler->sh_mem->curr_reading_line = 0;
    else
    {
        sh_mem_handler->sh_mem->curr_writing_line = 0;
        sh_mem_handler->sh_mem->finished_writing = FALSE ;
    }
}

void write_sh_mem(sh_mem_ADT sh_mem_handler, const char *msg)
{
    if (sh_mem_handler->sh_mem->curr_writing_line == MAX_LINES) // llegue al tope de lo q puedo escribir
    {
        error_exit("Out of memory", MEMORY_ERROR);
    }

    snprintf(sh_mem_handler->sh_mem->buff[sh_mem_handler->sh_mem->curr_writing_line++], MAX_BUF_SIZE, "%s", msg); //escribo msg en el sh_m, con max_buff_size de tamanio y %s de formay0

    sh_mem_handler->sh_mem->finished_writing = TRUE;

    if (sem_post(sh_mem_handler->semaphore) == -1)
        exit_error("Error writing in semaphore", SEMAPHORE_ERROR);
}

void read_sh_mem(sh_mem_ADT sh_mem_handler, char *buff)
{
    if (sem_wait(sh_mem_handler->semaphore) == -1)
        exit_error("Error writing in semaphore", SEMAPHORE_ERROR);
        
    snprintf(buff, MAX_BUF_SIZE, "%s", sh_mem_handler->sh_mem->buff[sh_mem_handler->sh_mem->curr_reading_line++]);
}

void finished_writing(sh_mem_ADT sh_mem_handler)
{
    sh_mem_handler->sh_mem->finished_writing = TRUE;
}

int can_read(sh_mem_ADT sh_mem_handler)
{
    return !sh_mem_handler->sh_mem->finished_writing;
}

void free_sh_mem_handler(sh_mem_ADT sh_mem_handler)
{
    if (sem_close(sh_mem_handler->semaphore) == -1)
        exit_error("Error closing semaphore", SEMAPHORE_ERROR);

    if (shmdt(sh_mem_handler->sh_mem) == -1)
        exit_error("Error detaching shared memory", SHARED_MEM_ERROR);

    if (shmctl(sh_mem_ID, IPC_RMID, 0) == -1)
        exit_error("Error in destroying", SHARED_MEM_ERROR);

    sem_unlink(SEM_NAME); // remueve el nombre del semaforo
    free(sh_mem_handler);
}