#include "masterADT.h"

#define BUF_SIZE 100
#define SLAVE_COUNT 4
#define FILES_PER_SLAVE 2

typedef struct masterCDT
{
    int pids[SLAVE_COUNT];
    int write_pipes[SLAVE_COUNT][2];
    int read_pipes[SLAVE_COUNT][2];
    int slave_processed[SLAVE_COUNT];

    int total_tasks;
    int assigned_tasks;
    int received_tasks;
    char **files;
    const char *result_path;

    // shmem_ADT sh_mem;
    // int sh_id;

} masterCDT;

masterADT new_master(char **files, int total_tasks, char * result_path)
{
    masterADT master = calloc(1, sizeof(masterCDT));
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
    for (i = 0; i < SLAVE_COUNT; i++)
    {
        if (pipe(master->read_pipes[i]) == -1)
        {
            error_exit("Error creating pipe\n", PIPE_ERROR);
        }
        if (pipe(master->write_pipes[i]) == -1)
        {
            error_exit("Error creating pipe\n", PIPE_ERROR);
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
            // Child

            int child_read_fd = master->write_pipes[i][0];
            int child_write_fd = master->read_pipes[i][1];

            int j;
            for (j = 0; j < SLAVE_COUNT; j++)
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

            dup2(child_read_fd, STDIN_FILENO);
            dup2(child_write_fd, STDOUT_FILENO);

            close(child_read_fd);
            close(child_write_fd);

            char *args[] = {"./slave.out", NULL};

            if (execvp(args[0], args) == -1)
            {
                error_exit("Error with exec on child process\n", EXEC_ERROR);
            }
        }
    }

    // Parent
    int j;
    for (j = 0; j < SLAVE_COUNT; j++)
    {
        close(master->write_pipes[j][0]);
        close(master->read_pipes[j][1]);
    }
}

void free_master(masterADT master)
{
    int i;
    for (i = 0; i < SLAVE_COUNT; i++)
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
    read(fd, &length, sizeof(int));
    read(fd, s, sizeof(char) * (length));
    s[length-1] = '\n';
    s[length] = 0;    

    printf(s);

}

static void close_pipes(masterADT master) {
    int i;
    for (i=0 ; i< SLAVE_COUNT ; i++) {
        close(master->read_pipes[i][0]);
        close(master->write_pipes[i][1]);
    }
}

void fetch_and_assign_new_tasks(masterADT master)
{
    fd_set rfd;

    FILE *result = fopen(master->result_path, "w");
    if (result == NULL) {

    }
        printf("received=%d, total=%d, assigned=%d\n", master->received_tasks, master->total_tasks, master->assigned_tasks);
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
    for (i = 0; i < SLAVE_COUNT; i++)
    {
        char *s = "Holaaaa\n";
        int len = strlen(s);
        if (write(master->write_pipes[i][1], &len, sizeof(int)) == -1)
        {
            error_exit("Error writing", WRITE_ERROR);
        }
        if (write(master->write_pipes[i][1], s, sizeof(char) * (len + 1)) == -1)
        {
            error_exit("Error writing", WRITE_ERROR);
        }
        close(master->write_pipes[i][1]);
    }
}

void test_read(masterADT master)
{
    int i;
    for (i = 0; i < SLAVE_COUNT; i++)
    {
        int length;
        char s[100] = {0};
        read(master->read_pipes[i][0], &length, sizeof(int));
        read(master->read_pipes[i][0], s, sizeof(char) * (length + 1));
        printf(s);
    }
    for (i = 0; i < SLAVE_COUNT; i++)
    {
        wait(NULL);
    }
    return;
}
