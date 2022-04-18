// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "masterADT.h"
#include "sh_mem_ADT.h"

#define BUF_SIZE 256
#define SLAVE_COUNT 4
#define FILES_PER_SLAVE 2

typedef struct masterCDT
{
    int pids[SLAVE_COUNT];
    int write_pipes[SLAVE_COUNT][2];
    int read_pipes[SLAVE_COUNT][2];

    int total_tasks;
    int assigned_tasks;
    int received_tasks;
    char **files;
    FILE * result_file;

    sh_mem_ADT sh_mem;
    int sh_mem_key;

} masterCDT;

masterADT new_master(char **files, int total_tasks, char * result_path)
{
    masterADT master = calloc(1, sizeof(masterCDT));
    if (master == NULL)
        error_exit("Error allocating memory", MALLOC_ERROR);

    master->total_tasks = total_tasks;
    master->files = files;

    master->sh_mem = new_sh_mem(&(master->sh_mem_key), WRITE);

    if (printf("%d", master->sh_mem_key) < 0)
        error_exit("Error printing shared mem id", WRITE_ERROR);

    char tasks_count_str[BUF_SIZE] = {0};

    if (snprintf(tasks_count_str, BUF_SIZE, "%d\n", total_tasks) < 0)
        error_exit("Error writing to string", WRITE_ERROR);

    write_sh_mem(master->sh_mem, tasks_count_str); // Lo primero que obtiene view por la shared mem es la cantidad total de lineas que va a leer

    master->result_file = fopen(result_path,"w");

    return master;
}

void init_slaves(masterADT master)
{

    int i;
    for (i = 0; i < SLAVE_COUNT; i++)
    {
        if (pipe(master->read_pipes[i]) == -1)
        {
            error_exit("Error creating pipe\n", FD_ERROR);
        }
        if (pipe(master->write_pipes[i]) == -1)
        {
            error_exit("Error creating pipe\n", FD_ERROR);
        }
    }

    for (i = 0; i < SLAVE_COUNT; i++)
    {
        master->pids[i] = fork();

        if (master->pids[i] == -1)
        {
            error_exit("Error creating process\n", FORK_ERROR);
        }

        if (master->pids[i] == 0)
        {
            // Hijo

            int child_read_fd = master->write_pipes[i][0];
            int child_write_fd = master->read_pipes[i][1];

            int j;
            for (j = 0; j < SLAVE_COUNT; j++)
            {
                if (j == i)
                {
                    if (close(master->write_pipes[j][1]) == -1 || close(master->read_pipes[j][0]) == -1)
                        error_exit("Error closing slave pipes", FD_ERROR);
                    
                }
                else
                {
                    if(close(master->write_pipes[j][0]) == -1 || close(master->write_pipes[j][1]) == -1
                    || close(master->read_pipes[j][0]) == -1 || close(master->read_pipes[j][1]) == -1)
                        error_exit("Error closing slave pipes", FD_ERROR);
                }
            }

            if (dup2(child_read_fd, STDIN_FILENO) == -1 || dup2(child_write_fd, STDOUT_FILENO) == -1)
                error_exit("Error duplicating fd", FD_ERROR);

            if (close(child_read_fd) == -1 || close(child_write_fd) == -1)
                error_exit("Error closing slave pipes", FD_ERROR);

            char *args[] = {"./slave.out", NULL};

            if (execvp(args[0], args) == -1)
            {
                error_exit("Error with exec on child process\n", EXEC_ERROR);
            }
        }
    }

    // Padre
    
    int j;
    for (j = 0; j < SLAVE_COUNT; j++)
    {
        close(master->write_pipes[j][0]);
        close(master->read_pipes[j][1]);
    }
}

void assign_task(masterADT master, int fd, char *file)
{
    int length = strlen(master->files[master->assigned_tasks]);

    if (write(fd, master->files[master->assigned_tasks], length) == -1)
        error_exit("Error writing to file descriptor", WRITE_ERROR);

    if (write(fd, "\n", 1) == -1)
        error_exit("Error writing to file descriptor", WRITE_ERROR);

    (master->assigned_tasks)++;
}

void assign_initial_tasks(masterADT master)
{
    int i, j;
    for (i = 0; i < SLAVE_COUNT; i++)
    {
        for (j = 0; j < FILES_PER_SLAVE && master->assigned_tasks < master->total_tasks; j++)
        {
            assign_task(master, master->write_pipes[i][1], master->files[master->assigned_tasks]);
        }
    }
}

void output_result(masterADT master, int fd, FILE *file)
{
    int length;
    char s[BUF_SIZE] = {0};
    if (read(fd, &length, sizeof(int)) == -1 || read(fd, s, sizeof(char) * (length)) == -1)
        error_exit("Error reading result from pipe", READ_ERROR);

    write_sh_mem(master->sh_mem, s);

        s[length] = '\n';

    if (fwrite(s, sizeof(char), length+1, master->result_file) == 0)
        error_exit("Error writing to file", WRITE_ERROR);   

}

static void close_pipes(masterADT master) {
    int i;
    for (i=0 ; i < SLAVE_COUNT ; i++) {
        if (close(master->read_pipes[i][0]) == -1)
            error_exit("Error closing master read pipe", FD_ERROR);

        if (close(master->write_pipes[i][1]) == -1)
            error_exit("Error closing master write pipe", FD_ERROR);
    }
}

void fetch_and_assign_new_tasks(masterADT master)
{
    fd_set rfd;

        while (master->received_tasks < master->total_tasks)
        {
            FD_ZERO(&rfd);

            int i;
            for (i = 0; i < SLAVE_COUNT; i++)
                FD_SET(master->read_pipes[i][0], &rfd);

            if (select(master->read_pipes[SLAVE_COUNT - 1][1] + 1, &rfd, NULL, NULL, NULL) == -1)
            {
                error_exit("error with select", SELECT_ERROR);
            }

            for (i = 0; i < SLAVE_COUNT; i++)
            {
                if (FD_ISSET(master->read_pipes[i][0], &rfd))
                {
                    output_result(master, master->read_pipes[i][0], NULL);
                    master->received_tasks++;

                    if (master->assigned_tasks < master->total_tasks)
                        assign_task(master, master->write_pipes[i][1], master->files[master->assigned_tasks]);

                    break;
                }
            }
            
        }
}

void free_master(masterADT master)
{
    close_pipes(master);

    if(fclose(master->result_file) == EOF)
        error_exit("Error closing file", FILE_ERROR);

    int i;
    for (i = 0; i < SLAVE_COUNT; i++)
    {
        wait(NULL);
    }
    free_sh_mem(master->sh_mem);
    free(master);
}

