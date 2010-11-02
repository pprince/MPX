#include "dos.h"
#include "mpx_cmd.h"
#include "mpx_util.h"
#include "mpx_r2.h"
#include "mpx_r3.h"
#include "mpx_r4.h"
#include "mpx_supt.h"
#include "mystdlib.h"
#include <string.h>
#include <stdio.h>

void loadProgram(int argc, char *argv[]){ //name,fileName,priority,path
	
	static int count;
	MEMDSC *tempMem;
	unsigned char temptop;
	int size,offset,priority;
	tcontext *tempContext;
	unsigned int tempCS,tempIP,*tempCS2,*tempIP2,*tempDS;
	ROOT *tempRQueue,*tempWSQueue;
	STACKDSC *temp;
		
	PCB *newPCB = allocate_PCB();
	tempMem=newPCB->memdsc;
		
	if((argc==5)||(127<priority<-128)&&((sys_check_program(argv[5],argv[3],&size,&offset))==0)){
		
		priority=atoi(argv[4]);
		
		if( count == ZERO ){ //If first process allocate queue
			tempRQueue = getRQueue();
			tempWSQueue = getWSQueue();
			tempRQueue = (ROOT*) sys_alloc_mem(sizeof(ROOT));
			tempWSQueue = (ROOT*) sys_alloc_mem(sizeof(ROOT));
		}
		
		setup_PCB(newPCB,argv[2],APPLICATION,SUSPENDED_READY,priority);

		
		tempMem->loadADDR=sys_alloc_mem(size);
		tempMem->execADDR=tempMem->loadADDR+offset;
		
		//make sure all registers are properly set
		temp= newPCB -> stackdsc;
		tempContext = (tcontext *) (temp -> top);
		tempContext ->IP=FP_OFF(tempMem->execADDR);
		tempContext ->CS=FP_SEG(tempMem->execADDR);	
		tempContext ->FLAGS = 0x200;
		tempContext ->ES = _ES;
		tempContext ->DS = _DS;

		
		sys_load_program(tempMem ->loadADDR,size,argv[5],argv[3]);
		
		insert_PCB(newPCB);	
		count++;//Update the number of times the function has run.
		
	}
	else{
		printf("Wrong or invalid arguments entered.");
	}
}

void terminateProcess(int argc, char *argv[]){

	if (argc == 2){
		char name[STRLEN];
		PCB *pointer;
		strcpy(name,argv[1]);
		pointer = find_PCB(name);
	
		if ( pointer != NULL){
			remove_PCB(pointer);
			free_PCB(pointer);
		}
	}
	
	else{
		printf("Wrong arguments entered.");
		return;
	}
}

