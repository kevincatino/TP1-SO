#define READ 0b1000
#define WRITE 0b0100

typedef struct sh_mem_CDT *sh_mem_ADT;


sh_mem_ADT new_sh_mem(int key, int flag);
void attach_to(sh_mem_ADT sh_mem_handler, int sh_mem_ID, int reader);
void write_sh_mem(sh_mem_ADT sh_mem_handler, const char *msg);
void read_sh_mem(sh_mem_ADT sh_mem_handler, char *buff); 
void finished_writing(sh_mem_ADT sh_mem_handler);
int can_read(sh_mem_ADT sh_mem_handler);
void free_sh_mem_handler(sh_mem_ADT sh_mem_handler);



