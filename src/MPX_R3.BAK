/***********************************************************************
	MPX: The MultiProgramming eXecutive
	Project to Accompany
	A Practical Approach to Operating Systems
	Malcolm G. Lane & James D. Mooney
	Copyright 1993, P.W.S. Kent Publishing Co., Boston, MA.

	File Name:	mpx_r3.h

	Author:	Nathaniel Clay and Nicholas Yanak
	Version: 1.1
	Date:  12/9/2010

	Purpose: This is the header for r3

		
	Environment: Windows XP 32 bit

************************************************************************/
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
/*
typedef struct IOCB {
	char *name;
	void ( funct *) (void); 
	PCB * IO_OP;
} IOCB;
*/

void interrupt sys_call(void);
void interrupt dispatch(void);
void mpxcmd_r3run(int argc, char *argv[]);
void mpxcmd_gor4(int argc, char *argv[]);
PCB *getHead_PCB();
#endif