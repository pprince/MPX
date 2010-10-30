#include "dos.h"
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
PCB *HEAD;
ELEM *TEMP;
ROOT *Root;
STACKDSC *STACK;


unsigned char sys_stack[SYS_STACK_SIZE];
unsigned short ss_save = NULL;
unsigned short sp_save = NULL;
unsigned short new_ss;
unsigned short new_sp;
tcontext *context_p;
tparams  *param_p;

void interrupt sys_call(void){
	
	
	
	param_p = ( tparams*)(24+ ((unsigned int)MK_FP( _SS, _SP)));//error in code here "verbatium" from manual?
	//context_p = (tcontext*)(MK_FP(_SS,_SP));
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
			remove_PCB(COP);// remove from queue if in queue then in either case dealocate memory
		COP = NULL;
	}

	
	// FIXME NO ERROR CHECKING 
	//context_p -> AX = 0;
	
	//CALL TO DISPATCHER
	dispatch();
	
}

void dispatch(void){
	Root = getRQueue();
	if ( sp_save == NULL ){
		ss_save = _SS;
		sp_save = _SP;
		if ( Root -> node != NULL ){
			HEAD = getHead_PCB();
			COP = copy_PCB( HEAD ); 
			remove_PCB(HEAD);
			STACK = COP -> stackdsc;
			_SS = FP_SEG(STACK -> base);
			_SP = FP_OFF(STACK -> top );
		}else{
			COP = NULL;
			_SS = ss_save;
			_SP = sp_save;
		}
		}
		//_iret;	
}

void mpxcmd_r3run(int argc, char *argv[]){
	
	
	PCB *test1;
	PCB *test2;
	PCB *test3;
	PCB *test4;
	PCB *test5;
	
	STACKDSC *stack1;
	STACKDSC *stack2;
	STACKDSC *stack3;
	STACKDSC *stack4;
	STACKDSC *stack5;
	
	tcontext *context1;
	tcontext *context2;
	tcontext *context3;
	tcontext *context4;
	tcontext *context5;
	
	char name1[10] = "test1";
	char name2[10] = "test2";
	char name3[10] = "test3";
	char name4[10] = "test4";
	char name5[10] = "test5";
	
	sys_set_vec(sys_call);
	
	test1 = allocate_PCB();
	test2 = allocate_PCB();
	test3 = allocate_PCB();
	test4 = allocate_PCB();
	test5 = allocate_PCB();
	
	stack1 = test1 -> stackdsc;
	stack2 = test2 -> stackdsc;
	stack3 = test3 -> stackdsc;
	stack4 = test4 -> stackdsc;
	stack5 = test5 -> stackdsc;
	
	stack1 -> top = stack1 -> base + STACKSIZE - sizeof(tcontext);
	stack2 -> top = stack2 -> base + STACKSIZE - sizeof(tcontext);
	stack3 -> top = stack3 -> base + STACKSIZE - sizeof(tcontext);
	stack4 -> top = stack4 -> base + STACKSIZE - sizeof(tcontext);
	stack5 -> top = stack4 -> base + STACKSIZE - sizeof(tcontext);
	
	context1 = (tcontext*) stack1 -> top;
	context2 = (tcontext*) stack2 -> top;
	context3 = (tcontext*) stack3 -> top;
	context4 = (tcontext*) stack4 -> top;
	context5 = (tcontext*) stack5 -> top;
	
	context1->DS = _DS;
	context1->ES = _ES;
	context1->CS = FP_SEG(&test1_R3);
	context1->IP = FP_OFF(&test1_R3);
	context1->FLAGS = 0x200;
	
	context2->DS = _DS;
	context2->ES = _ES;
	context2->CS = FP_SEG(&test2_R3);
	context2->IP = FP_OFF(&test2_R3);
	context2->FLAGS = 0x200;
	
	context3->DS = _DS;
	context3->ES = _ES;
	context3->CS = FP_SEG(&test3_R3);
	context3->IP = FP_OFF(&test3_R3);
	context3->FLAGS = 0x200;
	
	context4->DS = _DS;
	context4->ES = _ES;
	context4->CS = FP_SEG(&test4_R3);
	context4->IP = FP_OFF(&test4_R3);
	context4->FLAGS = 0x200;
	
	context5->DS = _DS;
	context5->ES = _ES;
	context5->CS = FP_SEG(&test5_R3);
	context5->IP = FP_OFF(&test5_R3);
	context5->FLAGS = 0x200;
	context5->DS = _DS;
	
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
	
//	dispatch();
	
	
}

