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

// loads a program into memory
void loadProgram(int argc, char *argv[]){ //name,fileName,priority,path
	
	// sets up variables
	static int count;
	MEMDSC *tempMem;
	unsigned char temptop;
	char *dir, *name, *filename;
	int size,offset,priority;
	tcontext *tempContext;
	unsigned int *tempCS,*tempIP;
	STACKDSC *temp;
	
	// initializes values
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
	
	err = sys_check_program(dir,filename,&size,&offset); 
	
	if((argc==5)||(127<=priority<=-128)&&( err==0)){ //checks for validity
		
		

	      /*	if( count == ZERO ){ //If first process allocate queue
			rQueue = (ROOT*) sys_alloc_mem(sizeof(ROOT));
			wsQueue = (ROOT*) sys_alloc_mem(sizeof(ROOT));
		}   */

		setup_PCB(newPCB,name,APPLICATION,SUSPENDED_READY,priority);


		// sets up the adressess
		newPCB->memdsc->loadADDR= sys_alloc_mem(size);
		newPCB->memdsc->execADDR=newPCB->memdsc->loadADDR + offset;// is this the correct address? 

		
		newPCB->memdsc->loadADDR= sys_alloc_mem(size);;
		newPCB->memdsc->execADDR=newPCB->memdsc->loadADDR + offset;

		
		//make sure all registers are properly set
		
		newPCB -> stackdsc-> top = newPCB -> stackdsc-> base + STACKSIZE - sizeof(tcontext);
		
		tempContext = (tcontext *) (newPCB -> stackdsc-> top);
		tempContext ->ES = _ES;
		tempContext ->DS = _DS;
		tempContext ->CS = FP_SEG(newPCB->memdsc->execADDR);
		tempContext ->IP = FP_OFF(newPCB->memdsc->execADDR);
		tempContext ->FLAGS = 0x200;
		
		// load the program into memory
		 err = sys_load_program(newPCB->memdsc->loadADDR,size,dir,filename);
		
		insert_PCB(newPCB);	// put pcb into a queue
		count++;//Update the number of times the function has run.
		
	}
	else{
		printf("Wrong or invalid arguments entered.");
	}
}

// removes process from memory
void terminateProcess(int argc, char *argv[]){

	if (argc == 2){ // checks for args then searches for process
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

