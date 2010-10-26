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
context *context_p;
params  *param_p;

void interrupt sys_call(void){
	
	
	
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
		remove_PCB(COP);  //check if in queue
		COP = NULL;
	}
	
	// SWITCH BACK OUT OF TEMP STACK
	
	// _SS = ss_save;
	// _SP = sp_save;
	
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

void mpxcmd_r3run( void ){
	
	sys_set_vec(sys_call);
	
	PCB *test1;
	PCB *test2;
	PCB *test3;
	PCB *test4;
	PCB *test5;
	
	MEMDSC *memdsc1;
	MEMDSC *memdsc2;
	MEMDSC *memdsc3;
	MEMDSC *memdsc4;
	MEMDSC *memdsc5;
	
	char name1[10] = "test1";
	char name2[10] = "test2";
	char name3[10] = "test3";
	char name4[10] = "test4";
	char name5[10] = "test5";
	
	test1 = allocate_PCB();
	test2 = allocate_PCB();
	test3 = allocate_PCB();
	test4 = allocate_PCB();
	test5 = allocate_PCB();
	
	memdsc1 = test1 -> memdsc;
	memdsc2 = test2 -> memdsc;
	memdsc3 = test3 -> memdsc;
	memdsc3 = test3 -> memdsc;
	memdsc4 = test4 -> memdsc;
	memdsc5 = test5 -> memdsc;
	
	memdsc1 -> execADDR = &test1_R3();
	memdsc2 -> execADDR = &test2_R3();
	memdsc3 -> execADDR = &test3_R3();
	memdsc4 -> execADDR = &test4_R3();
	memdsc5 -> execADDR = &test5_R3();
	
	
	setup_PCB(test1,name1,APPLICATION,READY, 1);
	setup_PCB(test2,name2,APPLICATION,READY, 2);
	setup_PCB(test3,name3,APPLICATION,READY, 3);
	setup_PCB(test4,name4,APPLICATION,READY, 4);
	setup_PCB(test5,name5,APPLICATION,READY, 5);

	insert_PCB(test1);
	insert_PCB(test2);
	insert_PCB(test3);
	insert_PCB(test4);
	insert_PCB(test5);
	
	dispatch();
	
	
}

