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

loadProgram(int argc, char *argv[]){ //name,fileName,priority,path
	
	static int count;
	MEMDSC *tempMem;
	int size,offset,priority;
	context tempContext;
	unsigned int tempCS,tempIP;
	
	PCB *newPCB = allocate_PCB();
	tempMem=newPCB->memdsc;
		
	if((argc==5)||(127<priority<-128)||((sys_check_program(argv[5],argv[3],size,offset))<=0)){
		
		priority=atoi(argv[4]);
		
		if( count == ZERO ){ //If first process allocate queue
			rQueue = (ROOT*) sys_alloc_mem(sizeof(ROOT));
			wsQueue = (ROOT*) sys_alloc_mem(sizeof(ROOT));
		}
		
		setup_PCB(newPCB,argv[2],APPLICATION,SUSPENDED_READY,priority);

		
		tempMem->loadADDR=sys_alloc_mem(size);
		tempMem->execADDR=loadADDR+offset;
		
		//make sure all registers are properly set
		tempCS=FP_SEG(tempMem->loadADDR);
		tempIP=FP_OFF(tempMem->execADDR);
		tempContext->CS=tempCS;
		tempContext->IP=tempIP;
		tempContext->DS=_DS;
		tempContext->ES=_ES;
		
		sys_load_program(*tempCS,100000,argv[5],argv[3]);
		
		insert_PCB(newPCB);	
		count++;//Update the number of times the function has run.
		
	}
	else{
		printf("Wrong or invalid arguments entered.");
	}
}

terminateProcess(int argc, char *argv[]){

	if (argc == 2){
		char name[STRLEN];
		PCB *pointer;
		strcpy(name,argv[1]);
		pointer = find_PCB(name);
	
		if ( pointer != NULL){
			remove_PCB(pointer);
			sys_free_mem(argv[1]);
		}
	}
	
	else(){
		printf("Wrong arguments entered.");
		return();
	}
}
