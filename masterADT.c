#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "masterADT.h"
#include <sys/wait.h>

#define MAX_SLAVE_QTY 4
#define FILES_PER_SLAVE 2
#define BUF_SIZE 100
#define RESULT_PATH "./results.txt"

typedef struct masterCDT
{
    int slave_count;
    int pids[MAX_SLAVE_QTY];
    int write_pipes[MAX_SLAVE_QTY][2];
    int read_pipes[MAX_SLAVE_QTY][2];
    int slave_processed[MAX_SLAVE_QTY];

    int total_tasks;
    int assigned_tasks;
    int received_tasks;
    char **files;
    const char *result_path;

    // shmem_ADT sh_mem;
    // int sh_id;

} masterCDT;

masterADT new_master(char **files, int total_tasks, const char *result_path, int shm_id)
{
    masterADT master = calloc(1, sizeof(masterCDT));
    master->slave_count = MAX_SLAVE_QTY;
    master->result_path = result_path;

    master->total_tasks = total_tasks;
    master->files = files;

    printf("total tasks = %d\n", master->total_tasks);

    // newMaster->sh_mem = otro ADT
    // newMaster->sh_mem_id = id del shared memory;

    return master;
}

void init_slaves(masterADT master)
{

    int i;
    for (i = 0; i < master->slave_count; i++)
    {
        if (pipe(master->read_pipes[i]) == -1)
        {
            printf("Error with creating pipe\n");
            exit(1);
        }
        if (pipe(master->write_pipes[i]) == -1)
        {
            printf("Error with creating pipe\n");
            exit(1);
        }
    }

    for (i = 0; i < master->slave_count; i++)
    {
        master->pids[i] = fork();

        if (master->pids[i] == -1)
        {
            printf("Error with creating process\n");
            exit(2);
        }

        if (master->pids[i] == 0)
        {
            // Child

            int childReadfd = master->write_pipes[i][0];
            int childWritefd = master->read_pipes[i][1];

            int j;
            for (j = 0; j < master->slave_count; j++)
            {
                if (j == i)
                {
                    close(master->write_pipes[j][1]);
                    close(master->read_pipes[j][0]);
                }
                else
                {
                    close(master->write_pipes[j][0]);
                    close(master->write_pipes[j][1]);
                    close(master->read_pipes[j][0]);
                    close(master->read_pipes[j][1]);
                }
            }

            dup2(childReadfd, STDIN_FILENO);
            dup2(childWritefd, STDOUT_FILENO);

            close(childReadfd);
            close(childWritefd);

            char *args[] = {"./child", NULL};

            if (execvp(args[0], args) == -1)
            {
                perror("Error al ejecutar programa hijo");
            }
        }
    }

    // Parent
    int j;
    for (j = 0; j < master->slave_count; j++)
    {
        close(master->write_pipes[j][0]);
        close(master->read_pipes[j][1]);
    }
}

void free_master(masterADT master)
{
    int i;
    for (i = 0; i < master->slave_count; i++)
    {
        wait(NULL);
    }
    printf("freeing\n");
    free(master);
    // free shared mem
}

void assign_task(masterADT master, int fd, char *file)
{
    int length = strlen(master->files[master->assigned_tasks]);
    printf("assigning %s to fd = %d\n", file, fd);
    write(fd, master->files[master->assigned_tasks], length);
    write(fd, "\n", 1);
    (master->assigned_tasks)++;
}

void assign_initial_tasks(masterADT master)
{
    int i, j;
    for (i = 0; i < master->slave_count; i++)
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
    read(fd, &length, sizeof(int));
    read(fd, s, sizeof(char) * (length));
    s[length-1] = '\n';
    s[length] = 0;    

    printf(s);

}

static void close_pipes(masterADT master) {
    int i;
    for (i=0 ; i< master->slave_count ; i++) {
        close(master->read_pipes[i][0]);
        close(master->write_pipes[i][1]);
    }
}

void fetch_and_assign_new_tasks(masterADT master)
{
    fd_set rfd;

    // FILE *result = fopen(master->result_path, "w");
    // if (result == NULL)
        // error
        printf("received=%d, total=%d, assigned=%d\n", master->received_tasks, master->total_tasks, master->assigned_tasks);
        while (master->received_tasks < master->total_tasks)
        {
            FD_ZERO(&rfd);

            int i;
            for (i = 0; i < master->slave_count; i++)
                FD_SET(master->read_pipes[i][0], &rfd);

            if (select(master->read_pipes[master->slave_count - 1][1] + 1, &rfd, NULL, NULL, NULL) == -1)
            {
                perror("select");
                exit(-1);
            }

            for (i = 0; i < master->slave_count; i++)
            {
                if (FD_ISSET(master->read_pipes[i][0], &rfd))
                {
                    printf("pipe %d listo para leer\n", master->read_pipes[i][0]);
                    output_result(master, master->read_pipes[i][0], NULL);
                    master->received_tasks++;

                    if (master->assigned_tasks < master->total_tasks)
                        assign_task(master, master->write_pipes[i][1], master->files[master->assigned_tasks]);

                    printf("tareas completadas: %d\n", master->received_tasks);
                    break;
                }
            }
            
        }
        
        close_pipes(master);
        printf("FINISHED!\n");

    // if (fclose(result) == -1) {
    //     exit(4);
    // }
    // error
}

void test_send(masterADT master)
{
    int i;
    for (i = 0; i < master->slave_count; i++)
    {
        char *s = "Holaaaa\n";
        int len = strlen(s);
        if (write(master->write_pipes[i][1], &len, sizeof(int)) == -1)
        {
            perror("Error at writing\n");
            exit(2);
        }
        if (write(master->write_pipes[i][1], s, sizeof(char) * (len + 1)) == -1)
        {
            perror("Error at writing\n");
            exit(2);
        }
        close(master->write_pipes[i][1]);
    }
}

void test_read(masterADT master)
{
    int i;
    for (i = 0; i < master->slave_count; i++)
    {
        int length;
        char s[100] = {0};
        read(master->read_pipes[i][0], &length, sizeof(int));
        read(master->read_pipes[i][0], s, sizeof(char) * (length + 1));
        printf(s);
    }
    for (i = 0; i < master->slave_count; i++)
    {
        wait(NULL);
    }
    return;
}
