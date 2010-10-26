#ifndef MPX_R2_HFILE
#define MPX_R2_HFILE
/* Symbolic Constants */
#define RUNNING  0///< state is Defined as 0
#define READY   1///< state is Defined as  1
#define BLOCKED 2 ///< state is defined as 2

#define SUSPENDED_READY    3 ///< is defined by 3
#define SUSPENDED_BLOCKED  4 ///< is defined by 4

#define SYSTEM  1 ///< is defined as 1
#define APPLICATION  0  ///< is defined as 0 

#define STACKSIZE   1024 ///< is the size of the stack in Bytes
#define STRLEN    16///< is the length of a string for name

#define PORDR  1 ///< is the Priority Order flag
#define FIFO   0 ///< is the First In First Out Order flag
#define ZERO   0

#define	MAX_LINE	1024

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
PCB *allocate_PCB(void);
int free_PCB( PCB *pointer);
int setup_PCB( PCB *pointer, char *name, int classType, int state, int priority );
void insert_PCB(PCB *PCBpointer/*< pointer to a PCB to insert*/ );
void insert_PORDR( PCB *PCBpointer, ROOT *quequeROOT );
void insert_FIFO( PCB *PCBpointer, ROOT *quequeROOT);
PCB *find_PCB( char *name);
void mpxcmd_create_PCB(int argc, char *argv[]);
void mpxcmd_delete_PCB(int argc, char *argv[]);
void mpxcmd_block(int argc, char *argv[]);
void mpxcmd_unblock(int argc, char *argv[]);
void mpxcmd_suspend(int argc, char *argv[]);
void mpxcmd_resume(int argc, char *argv[]);
void mpxcmd_setPriority(int argc, char *argv[]);
void mpxcmd_show_PCB(int argc, char *argv[]);
void mpxcmd_showAll_PCB(int argc, char *argv[]);
void mpxcmd_showReady_PCB(int argc, char *argv[]);
void mpxcmd_showBlocked_PCB(int argc, char *argv[]);
#endif