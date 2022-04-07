typedef struct masterCDT * masterADT;

masterADT new_master(char ** files, int total_tasks, int shm_id);
void init_slaves(masterADT master);
void clean_resources(masterADT master);
void test_send(masterADT master);
void test_read(masterADT master);