#include "mpx_supt.h"
#include "mpx_util.h"
#include "mpx_cmd.h"
#include "mpx_r4.h"
#include "mpx_r3.h"
#include "mpx_r2.h"



/**
* Entry Point of MPX. 
* This is the entry point of MPX, it calls the mpx_command_loop. The Command Loop function
* starts the display of the Welcome Message and the inital home screen of MPX.
*/
void main(){
	int err;
	PCB *command_loop;
	STACKDSC *command_stack;
	tcontext *command_context;
	char command[20] = "Command_Hand";
	sys_init( MODULE_R4 ); //System initilization
	sys_set_vec(sys_call);
	
	
	
	
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
	
	dispatch();
	printf("exit sucess");
	errorDecode(err);
}
