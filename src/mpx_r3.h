#ifndef MPX_R3_HFILE
#define MPX_R3_HFILE

#define SYS_STACK_SIZE 200
 
typedef struct context {
	unsigned int BP, DI, SI, DS, ES;
	unsigned int DX, CX, BX, AX;
	unsigned int IP, CS, FLAGS;
} tcontext;

typedef struct params { 
int op_code;
int device_id;
unsigned char *buf_addr;
int *cont_addr;
} tparams;

void interrupt sys_call(void);
void dispatch(void);
void mpxcmd_r3run(int argc, char *argv[]);
PCB *getHead_PCB();
PCB *copCOPY_PCB(PCB *pointer);
#endif