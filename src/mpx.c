/***********************************************************************
	MPX: The MultiProgramming eXecutive
	Project to Accompany
	A Practical Approach to Operating Systems
	Malcolm G. Lane & James D. Mooney
	Copyright 1993, P.W.S. Kent Publishing Co., Boston, MA.

	File Name:	mpx.c

	Author:	Nathaniel Clay and Nicholas Yanak
	Version: 1.1
	Date:  12/9/2010

	Purpose: This is the startup program. It sets initial values
	and then calls the dispatcher.

		
	Environment: Windows XP 32 bit

************************************************************************/

#include <dos.h>
#include "mpx_supt.h"
#include "mpx_util.h"
#include "mpx_cmd.h"
#include "mpx_r4.h"
#include "mpx_r3.h"
#include "mpx_r2.h"
#include "mpx_r5.h"
#include "TRMDRIVE.H"


/**
* Entry Point of MPX. 
* This is the entry point of MPX, it calls the mpx_command_loop. The Command Loop function
* starts the display of the Welcome Message and the inital home screen of MPX.
*/
void main(){
	int err;
	PCB *command_loop, *idlePCB;
	STACKDSC *command_stack;
	tcontext *command_context;
	tcontext *tempContext;
	int offset;
	int sizex;
	int priority;
	int eventFlag, eventtFlag;
	char dir[20] = "proc";
	char name[20] = "idle";
	char filename[20] = "IDLE";
	char command[20] = "Command_Hand";
	sys_init( MODULE_R4 ); //System initilization
	sys_set_vec(sys_call);

	//Open Device Drivers
	com_open( (int *) eventFlag, 1200);
	trm_open(  (int *) eventtFlag );

	// Command Handler loop insertion
	command_loop = allocate_PCB();

	command_stack = command_loop -> stackdsc;

	command_stack->top = command_stack->base + STACKSIZE - sizeof(tcontext);

	command_context = (tcontext*) command_stack->top;

	command_context->DS = _DS;
	command_context->ES = _ES;
	command_context->CS = FP_SEG(&mpx_command_loop);
	command_context->IP = FP_OFF(&mpx_command_loop);
	command_context->FLAGS = 0x200;

	setup_PCB(command_loop,command,SYSTEM,READY,-127);

	insert_PCB(command_loop);
	
	//IDLE Process insertion 
	
	sys_check_program(dir,filename,&sizex,&offset);

	idlePCB = allocate_PCB();
	setup_PCB(idlePCB,name,APPLICATION,READY,127);

	idlePCB->memdsc->loadADDR = sys_alloc_mem(sizex);
	idlePCB->memdsc->execADDR = idlePCB->memdsc->loadADDR + offset;

	idlePCB->stackdsc->top = idlePCB->stackdsc->base + STACKSIZE - sizeof(tcontext);

	tempContext = (tcontext *) (idlePCB->stackdsc->top);
	tempContext->ES = _ES;
	tempContext->DS = _DS;
	tempContext->CS = FP_SEG(idlePCB->memdsc->execADDR);
	tempContext->IP = FP_OFF(idlePCB->memdsc->execADDR);
	tempContext->FLAGS = 0x200;

	sys_load_program(idlePCB->memdsc->loadADDR,sizex,dir,filename);

	insert_PCB(idlePCB);


	dispatch();
	printf("exit sucess");
	//mpxprompt_anykey();
	sys_exit();
}
