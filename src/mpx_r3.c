#include "mpx_cmd.h"
#include "mpx_util.h"
#include "mpx_r2.h"
#include "mpx_r3.h"
#include "procs-r3.c"
#include "mpx_supt.h"
#include "mystdlib.h"
#include <string.h>
#include <stdio.h>

PCB *COP;
ELEM *TEMP;
STACKDSC *STACK;


byte sys_stack[SYS_STACK_SIZE];
unsigned short ss_save = NULL;
unsigned short sp_save = NULL;
unsigned short new_ss;
unsigned short new_sp;

void interrupt sys_call(void){
	
	context *context_p;
	params  *param_p;
	
	param_p = (params*)( MK_FP(_SS, _SP) + sizeof(context));
	context_p = (context*)(MK_FP(_SS,_SP));
	//SWITCH TO TEMP STACK
	ss_save = _SS;
	sp_save = _SP;
	new_ss = FP_SEG(sys_stack);
	new_sp = FP_OFF(sys_stack);
	new_sp += SYS_STACK_SIZE;
	
	if ( param_p -> op_code == IDLE ){
		if ( COP != NULL){
			if( COP -> state == READY || COP -> state == RUNNING ) COP -> state = BLOCKED;
			insert_PCB(COP);
			COP = NULL;
		}
	}
	if( param_p -> op_code == EXIT ){
		remove_PCB(COP);
		COP = NULL;
	}
	
	//SWITCH BACK OUT OF TEMP STACK
	
	_SS = ss_save;
	_SP = sp_save;
	
	// FIXME NO ERROR CHECKING 
	context_p -> AX = 0;
	
	//CALL TO DISPATCHER
	dispatch();
	
}

void dispatch(void){
	
	if sp_save == NULL{
		ss_save = _SS;
		sp_save = _SP;
		if ( rQueue -> node != NULL ){
			TEMP = rQueue -> node;
			COP = copy_PCB( TEMP -> process ); 
			remove_PCB(TEMP -> process);
			STACK = COP -> stackdsc;
			_SS = FP_SEG(STACK -> base);
			_SP = FP_OFF(STACK -> top );
		}else{
			COP = NULL;
			_SS = ss_save;
			_SP = sp_save;
		}
		iret;	
}