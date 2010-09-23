#include "mpx_r2.h"
#include "mpx_supt.h"
#include "mpx_supt.c"
#include "mystdlib.h"
#include <string.h>
#include <stdio.h>

/** Allocates the memory for a new Process Control Block and returns 
	the pointer to the new PCB location in memory.
*/

PCB *alloocate_PCB( void ){
	PCB *newPCB; //< pointer to the new PCB
	MEMDSC *newMemDsc;//< pointer to the Memory Descriptor
	STACKDSC *newStackDsc;//<pointer to the stack descriptor
	unsigned char *stack; //< pointer to the stack low address
	
	// Allocate memory to each of the disctenct parts of the PCB
	newStackDsc = (STACKDSC*) sys_alloc_mem(sizeof(STACKDSC));
	newMemDsc = (MEMDSC*) sys_alloc_mem(sizeof(MEMDSC));
	newPCB = (PCB*) sys_alloc_mem(sizeof(PCB));
	
	if ( stack == NULL || 
		 newStackDsc == NULL || 
		 newMemDsc == NULL || 
		 newPCB == NULL ) return NULL;
	
	//Bundling Opereations of Stack Descripter Bellow
	newPCB -> stackdsc = newStackDsc;  // stack descriptor is placed in the PCB
	
	//Bundling Operations of Memory Descriptor 
	newPCB -> memdsc = newMemDsc; // memory descriptor is placed in the PCB
	
	return newPCB;
	
};
/**
This function releases all allocated memory related to a PCB. 
*/
int free_PCB( PCB *pointer /*< is a pointer to a PCB [in] */ ){
	STACKDSC *stackdscptr = pointer -> stackdsc;//< is a pointer to the stack descriptor
	unsigned char *stack = stackdscptr -> base;//< is a pointer to the base location of the stack
	MEMDSC *memptr = pointer -> memdsc;//< is a pointer to a Memory Descriptor 
	
	int err;//< holder for error capture on use of sys_free_mem
	
	//Free Stack First
	err = sys_free_mem(stack);
	if( err < 0 ) return err;
	//Second free Stack Descriptor
	err = sys_free_mem(stackdscptr);
	if( err < 0 ) return err;
	//Third free Memory Descriptor
	err = sys_free_mem(memptr);
	if( err < 0 ) return err;
	//Finaly free Process Control block
	err = sys_free_mem(pointer);
	if(err < 0 ) return err;
	
	return 0; //freed mem ok
}

/** This Function initializes the contents of a PCB. */

void setup_PCB( PCB *pointer, char *name,int classType){
	int i;
	unsigned char *stack;
	char *nPtr = pointer -> name;
	//Allocate the stack
	stack = (unsigned char*) sys_mem_alloc(STACKSIZE);
	
	//copy name into the PCB
	strncpy(nPtr, name,STRLEN);
	
	//set priority of PCB ( this is for MOD 2 Only, comment out after Mod 2) 
	pointer -> priority =  READY;
	//Ucomment after mod 2
	// pointer -> priority = priority;
	
	//set classTyepe
	pointer -> classType = classType;
	//Setup Memory Descriptor with Default Values for Module 2
	pointer -> memdsc -> size = 0;
	pointer -> memdsc -> loadADDR = NULL;
	pointer -> memdsc -> execADDR = NULL;
	
	//Setup the Stack
	for( i = 0; i < STACKSIZE; i++)  *(stack + i) = 0; //ZERO out Stack to aid in debug....
	pointer -> stackdsc -> base = stack[STACKSIZE-1]; // x86 arch Stacks start at the Higest value 
	pointer -> stackdsc -> top  = stack[STACKSIZE-1];// and go to lowest or n - 2 for Word alloc 
	
	
	
}