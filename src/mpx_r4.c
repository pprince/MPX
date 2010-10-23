#include "mpx_cmd.h"
#include "mpx_util.h"
#include "mpx_r2.h"
#include "mpx_r3.h"
#include "procs-r3.c"
#include "mpx_supt.h"
#include "mystdlib.h"
#include <string.h>
#include <stdio.h>

loadProgram(int argc, char *argv[]){ //name,fileName,priority
	
	static int count;
	char name[STRLEN],line[MAX_LINE];
	MEMDSC *tempMem;
	int size,type,priority;
	context tempContext;
	PCB *newPCB = allocate_PCB();
	tempMem=newPCB->memdsc;
		
	if((argc==3)||(127<priority<-128)||((size=sys_check_program())<=0)){
		
		priority=atoi(argv[2]);
		
		if( count == ZERO ){ //If first process allocate queue
			rQueue = (ROOT*) sys_alloc_mem(sizeof(ROOT));
			wsQueue = (ROOT*) sys_alloc_mem(sizeof(ROOT));
		}
		
		setup_PCB(newPCB,argv[0],APPLICATION,SUSPENDED_READY,priority);

		
		tempMem->loadADDR=sys_alloc_mem(size);
		tempMem->execADDR=loadADDR+size;
		
		//make sure all registers are properly set
		tempContext->CS=FP_SEG(tempMem->loadADDR);
		tempContext->IP=FP_OFF(tempMem->execADDR);
		tempContext->DS=_DS;
		tempContext->ES=_ES;
		
		//load program	
		insert_PCB(newPCB);	
		count++;//Update the number of times the function has run.
		sys_load_program();
		
	}
	else{
		printf("Wrong arguments entered.");
	}
}

terminateProcess(int argc, char *argv[]){

	if (argc == 1){
		char name[STRLEN];
		PCB *pointer;
		strcpy(name,argv[1]);
		pointer = find_PCB(name);
	
		if ( pointer != NULL){
			remove_PCB(pointer);}

		sys_free_mem()
	
	}
	
	else(){
		printf("Wrong arguments entered.");
		return();
	}
}

dispatch(int argc, char *argv[]){
	//waiting for dispatch from r3
}
