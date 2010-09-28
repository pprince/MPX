#ifndef MPX_R2_HFILE
#define MPX_R2_HFILE
/* Symbolic Constants */
#define RUNNING 0x02///< state is Defined as 0x02H
#define READY   0x04///< state is Defined as 0x04H
#define BLOCKED 0x08///< state is defined as 0x08H
#define SUSPEND  -0xFF///<is a constant to place a process in a suspened state.

#define SUSPENDED_READY    -0xFB ///< is defined by SUSPEND + READY
#define SUSPENDED_BLOCKED  -0xF7 ///< is defined by SUSSPEND + BLOCKED

#define SYSTEM  1 ///< is defined as 1
#define APPLICATION  0  ///< is defined as 0 

#define STACKSIZE   1024 ///< is the size of the stack in Bytes
#define STRLEN    16///< is the length of a string for name

#define PORDR  1 ///< is the Priority Order flag
#define FIFO   0 ///< is the First In First Out Order flag
/* Type Definitions and Structures */
typedef struct mem{
	int size;///< Number of words in memory
	unsigned char *loadADDR;///< Address to load data to 
	unsigned char *execADDR;///< Address of first INSTRUCTION
}MEMDSC;

typedef struct stack{
	unsigned char *top; ///< pointer to the top of the stack
	unsigned char *base;///< pointer to the bottom of the stack
}STACKDSC;

typedef struct process{
	char name[STRLEN]; ///< character array containing 16 characters plus space for null
	signed char classType; ///< class of process APPLICATION or SYSTEM
	signed char priority;///< process priority ranges from -128 to +127
	signed char state;///< stores the current states of the process
	MEMDSC *memdsc;///< stores the description of the ADDRESS SPACE for the process	
	STACKDSC *stackdsc;///< stores the description of the stack for each process;
}PCB;

typedef struct page{
	PCB *process; ///< pointer to the PCB structure
	unsigned char *left; ///< pointer to the left PCB structure
	unsigned char *right;///< pointer to the right PCB structure
}ELEM;

typedef struct root{
	int count;
	unsigned char *node;
}ROOT;


/* Functions Dec*/
PCB *alloocate_PCB(void);
int free_PCB( PCB *pointer);
void setup_PCB( PCB *pointer, char *name,int classType);

#endif