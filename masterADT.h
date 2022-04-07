typedef struct masterCDT * masterADT;

masterADT new_master(char **files, int total_tasks, const char * result_path, int shm_id);
void init_slaves(masterADT master);
void clean_resources(masterADT master);
void test_send(masterADT master);
void test_read(masterADT master);
void assign_initial_tasks(masterADT master);
void fetch_and_assign_new_tasks(masterADT master);