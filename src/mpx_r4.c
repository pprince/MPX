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

extern ROOT *rQueue, *wsQueue; //link in the values for these in r2
void * loadAddr;
void loadProgram(int argc, char *argv[]){ //name,fileName,priority,path
	
	static int count;
	MEMDSC *tempMem;
	unsigned char temptop;
	char *dir, *name, *filename;
	int size,offset,priority;
	tcontext *tempContext;
	unsigned int tempCS,tempIP,*tempCS2,*tempIP2,*tempDS;
	ROOT *tempRQueue,*tempWSQueue;
	STACKDSC *temp;
	
	int err = 0;
	PCB *newPCB = allocate_PCB();
	tempMem=newPCB->memdsc;
	dir = (char*) sys_alloc_mem(30 * sizeof(char));
	name = (char*) sys_alloc_mem(30 * sizeof(char));
	filename = (char*) sys_alloc_mem(30 * sizeof(char));
	strcpy(dir,argv[4]);
	strcpy(name,argv[1]);
	strcpy(filename,argv[2]);
	priority = atoi(argv[3]);
	
	err =  = sys_check_program(dir,filename,&size,&offset);
	if((argc==5)||(127<=priority<=-128)&&( err==0)){
		
		

		if( count == ZERO ){ //If first process allocate queue
			tempRQueue = getRQueue();
			tempWSQueue = getWSQueue();
			tempRQueue = (ROOT*) sys_alloc_mem(sizeof(ROOT));
			tempWSQueue = (ROOT*) sys_alloc_mem(sizeof(ROOT));
		}

		setup_PCB(newPCB,name,APPLICATION,SUSPENDED_READY,priority);


		loadAddr = sys_alloc_mem(size);
		tempMem->loadADDR= loadAddr;
		tempMem->execADDR=tempMem->loadADDR + offset;
		
		//make sure all registers are properly set
		temp= newPCB -> stackdsc;
		temp -> top = temp -> base + STACKSIZE - sizeof(tcontext);
		
		tempContext = (tcontext *) (temp -> top);
		tempContext ->IP=FP_OFF(tempMem->execADDR);
		tempContext ->CS=FP_SEG(tempMem->execADDR);	
		tempContext ->FLAGS = 0x200;
		tempContext ->ES = _ES;
		tempContext ->DS = _DS;

		
		 err = sys_load_program(tempMem ->loadADDR,size,dir,filename);
		
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

