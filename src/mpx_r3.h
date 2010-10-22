#ifndef MPX_R3_HFILE
#define MPX_R3_HFILE

#define SYS_STACK_SIZE 200
 
typedef struct context {
	unsigned int BP, DI, SI, DS, ES;
	unsigned int DX, CX, BX, AX;
	unsigned int IP, CS, FLAGS;
} context;

typedef struct params { 
int op_code;
int device_id;
byte *buf_addr;
int *cont_addr;
} params;

void interrupt sys_call(void);
void dispatch(void);


#endif