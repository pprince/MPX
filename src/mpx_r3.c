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

PCB *getHead_PCB();

PCB *cop;
PCB *HEAD;
ELEM *TEMP;
ROOT *Root;
STACKDSC *STACK;

extern ROOT *rQueue, *wsQueue; //link in the values for these in r2

unsigned char sys_stack[SYS_STACK_SIZE];
unsigned short ss_save = NULL;
unsigned short sp_save = NULL;
unsigned short new_ss = NULL;
unsigned short new_sp = NULL;
tcontext *context_p;
tparams  *param_p;
//IOCB termCB;
//IOCB comCB;


void interrupt sys_call(void){
	

	cop-> stackdsc -> top = (unsigned char *) MK_FP(_SS, _SP);
	param_p = ( tparams*)(sizeof(tcontext) + cop -> stackdsc -> top);//code supplied by GA bryan
	context_p = (tcontext*) cop -> stackdsc -> top;
	//SWITCH TO TEMP STACK by storing all of your variables in memory
	
	//replaces SS and SP
	new_ss = FP_SEG(sys_stack);
	new_sp = FP_OFF(sys_stack);
	new_sp += SYS_STACK_SIZE;
	_SS = new_ss;
	_SP = new_sp;
	
	// if the idle opcode is sent then change staate to ready and insert into queue
	if ( param_p -> op_code == IDLE ){
			cop -> state = READY;
			insert_PCB(cop);
			cop = NULL;
	}
	
	// if the exit opcode is sent then remove and free the pcb
	if( param_p -> op_code == EXIT ){
		remove_PCB(cop);
		free_PCB(cop);
		cop = NULL;
	}
	
	// if ( param_p -> op_code == READ ){
		// if( param_p -> device_id == TERMINAL ){
		
		// }
		// if ( param_p -> device_id == COM_PORT ){
		
		// }
	// }
	// if ( param_p -> op_code == WRITE ){
		// if( param_p -> device_id == TERMINAL ){
		
		// }
		// if( param_p -> device_id == COM_PORT ){
		
		// }
		
	// }
	
	

	
	// FIXME NO ERROR CHECKING 
	//context_p -> AX = 0;
	
	//CALL TO DISPATCHER
	dispatch();
	
}

void interrupt dispatch(void){
	
	if ( sp_save == NULL ){ //saves the SS and SP from being overwritten if not already done
		ss_save = _SS;
		sp_save = _SP;
		}
		HEAD = getHead_PCB();
		//STACK = HEAD -> stackdsc;
		
		// get a process from the ready queue then set the ss and sp to execute the new process
		if ( HEAD != NULL ){
			cop = HEAD;
			cop -> state = READY;
			remove_PCB(HEAD);
			STACK = cop -> stackdsc;
			new_ss = FP_SEG(STACK -> top);
			new_sp = FP_OFF(STACK -> top );
			_SS = new_ss;
			_SP = new_sp;
		}else{ // if no process left return
			cop = NULL;
			_SS = ss_save;
			_SP = sp_save;
			ss_save = NULL;
			sp_save = NULL;
		}
		//_iret;	
}

	// returns the head pcb of the ready queue
PCB *getHead_PCB(){
		ELEM *incr;
		PCB  *pointer= NULL;
		
		incr = rQueue -> node;//set node to the first node in the queque
		if( incr  != NULL ){
			pointer = incr -> process;
			incr = incr -> right; // progress forward to the right of the queque			incr = incr -> right; // progress forward to the right of the queque
		}
		return pointer;
}

// used to test r3 test processes
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
	stack5 -> top = stack5 -> base + STACKSIZE - sizeof(tcontext); 
	
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
	
	
	mpx_cls();
	dispatch();
	mpxprompt_anykey();
	
}

// used to test r4 test processes
void mpxcmd_gor4(int argc, char *argv[]){
	mpx_cls();
	dispatch();
	mpxprompt_anykey();
}


