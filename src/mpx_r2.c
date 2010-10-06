#include "mpx_r2.h"
#include "mpx_supt.h"
#include "mystdlib.h"
#include <string.h>
#include <stdio.h>


ROOT *rQueue; // Remember to reserve memory for these
ROOT *wsQueue;


/** Allocates the memory for a new Process Control Block and returns 
	the pointer to the new PCB location in memory.
*/
PCB *aloocate_PCB( void ){
	PCB *newPCB; ///< pointer to the new PCB
	MEMDSC *newMemDsc;///< pointer to the Memory Descriptor
	STACKDSC *newStackDsc;///<pointer to the stack descriptor
	unsigned char *stack; ///< pointer to the stack low address
	
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
int free_PCB( PCB *pointer /*< [in] is a pointer to a PCB  */ ){
	STACKDSC *stackdscptr = pointer -> stackdsc;///< is a pointer to the stack descriptor
	unsigned char *stack = stackdscptr -> base;///< is a pointer to the base location of the stack
	MEMDSC *memptr = pointer -> memdsc;///< is a pointer to a Memory Descriptor 
	
	int err;///< holder for error capture on use of sys_free_mem
	
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

void setup_PCB( PCB *pointer ){
	int i;///< counter varable for loop 
	unsigned char *stack;///< pointer to what will become the bottom of the stack 
	//Allocate the stack
	stack = (unsigned char*) sys_mem_alloc(STACKSIZE);
	
	
		
		
	//set priority of PCB ( this is for MOD 2 Only, comment out after Mod 2) 
	pointer -> priority =  READY;
	//Ucomment after mod 2
	// pointer -> priority = priority;
	
	//Setup Memory Descriptor with Default Values for Module 2
	pointer -> memdsc -> size = 0;
	pointer -> memdsc -> loadADDR = NULL;
	pointer -> memdsc -> execADDR = NULL;
	
	//Setup the Stack
	for( i = 0; i < STACKSIZE; i++)  *(stack + i) = 0; //ZERO out Stack to aid in debug....
	pointer -> stackdsc -> base = stack; // x86 arch Stacks start at the Higest value 
	pointer -> stackdsc -> top  = stack[STACKSIZE-1];// and go to lowest or n - 2 for Word alloc 
	
}

void insert_PCB(PCB *PCBpointer/*< pointer to a PCB to insert*/ ){ 
   int ORD;
   if ( PCBpointer -> state == READY || PCBpointer -> state == RUNNING ){
		ORD  = FIFO;
	}
	if( PCBpointer -> state == BLOCKED || 
		PCBpointer -> state == SUSPENDED_READY || 
		PCBpointer -> state == SUSPENDED_BLOCKED ){
		ORD  = PORDR;
	}
   switch(ORD){
		case PORDR:
			insert_PORDR(PCBpointer,wsQueue);
			break;
		case FIFO:
			insert_FIFO(PCBpointer,rQueue);
			break;
		default:
			printf("ORDER not Valid");
			break;
		};
}

void insert_PORDR( PCB *PCBpointer, ROOT *queueROOT ){ //FIXME: NO ERROR CHECKING
	ELEM *node; // declare node of type element
	ELEM *incr;
	node = sys_alloc_mem( sizeof(ELEM)); // allocate Memory for node
	node -> process = PCBpointer;// add the PCB to the node
	
	if( queueROOT -> node = NULL ){ // if this is the first element ever in the queque
		node -> left = NULL;
		node -> right = NULL;
		queueROOT -> node = node; // Set the first element in the queque to node of Type Element
		queueROOT -> count += 1; // increase count by one
		return; //exit out first node is in queque. 
	}
	incr = queueROOT -> node; //set node to the first node in the queque
	while ( (incr -> process -> priority <= node -> process -> priority) && incr != NULL ){ // Process with the lowest priority goes first 
			incr = incr -> right; // progrees to the right 
	}
	/* There are three cases to check for head, tail, and middle*/
	
	/*head case*/
	if( incr -> left == NULL && incr->right != NULL ){ 
		node->left = NULL;
		node->right = incr;
		incr->left = node;
		queueROOT -> node = node; //set quequeROOT to new head
		queueROOT ->count +=1;
	}
	
	/*tail case*/
	if( incr -> left != NULL && incr->right == NULL ){
		node-> left = &incr;
		node-> right = NULL;
		incr->right = &node;
		queueROOT->count +=1;
	}
	
	/*middle case*/
	if( incr -> left != NULL && incr->right != NULL){
		node->right = incr->right;
		incr->right = node;
		node->left = incr;
		queueROOT->count +=1;
	}
	return;
}
/** In this function we grow the queque to the right no matter of the Priority of the PCB.*/ 
void insert_FIFO( PCB *PCBpointer, ROOT *queueROOT){ //FIXME: NO ERROR HANDLING
	ELEM *node; // declare node of type element
	ELEM *incr;
	node = sys_alloc_mem( sizeof(ELEM)); // allocate Memory for node
	node -> process = PCBpointer;// add the PCB to the node
	
	if( quequeROOT -> node = NULL ){ // if this is the first element ever in the queque
		node -> left = NULL; // set the link left to null
		node -> right = NULL;// set the link right to null
		queueROOT -> node = node; // Set the first element in the queque to node of Type Element
		queueROOT -> count += 1; // increase count by one
		return; //exit out first node is in queque. 
	}
	
	
	/* INSERT INTO THE QUEQUE IN FIFO ORDER*/
	incr = queueROOT -> node; //set node to the first node in the queque
	while( incr -> right != NULL ){
		incr = incr -> right; // progress forward to the right of the queque
	}
	 node -> left = incr; //set left to previous node
	 node -> right = NULL; // set right to null 
	 quequeROOT -> count += 1; // increase count by one as the size of the queque has grown by one
	 
	 return;

}

PCB *find_pcb( char *name){
	incr =  rQueue -> node; //set node to the first node in the queque
	while ( strcmp(name,incr -> process -> name ) != 0 && incr != NULL){ // Process with the lowest priority goes first 
			incr= incr -> right; // progrees to the right 
	}
	if (incr == NULL ){
	incr =  wsQueue -> node; //set node to the first node in the queque
	while ( strcmp(name,incr -> process -> name ) != 0 && incr != NULL){ // Process with the lowest priority goes first 
			incr= incr -> right; // progrees to the right 
	}
	}
	
	return incr;

}

void mpxcmd_create_PCB(int argc, char *argv[]){
	static int count = ZERO;
		int isValid = ZERO;
	if( count == ZERO ){
		rQueue = (ROOT*) sys_alloc_mem(sizeof(ROOT));
		wsQueue = (ROOT*) sys_alloc_mem(sizeof(ROOT));
	}
	
	PCB *newPCB = allocate_PCB();
	
	printf("Process Name: \n");
	sys_req(TERMINAL,READ,newPCB -> name,STRLEN);
	printf("Process Class Type ( Application 0 or System  1): \n" );
	sys_req(TERMINAL,READ,newPCB -> classType,2);
	printf("Process Priority (-128 to 127): /n");
	sys_req(TERMINAL,READ,newPCB -> priority,4);
	
	while( isValid !=  1 ){
		//check name validity
		if ( find_PCB( newPCB -> name ) != NULL ) {
			printf("Process Name NOT VALID");
			printf("Process Name: \n");
			sys_req(TERMINAL,READ,newPCB -> name,STRLEN);
			isValid = 0;
		}else{
			isValid = 1;
		}
		//check if system or type
		if( newPCB -> classType == APPLICATION || newPCB -> classType == SYSTEM ){
			isValid = 1;
		}else{
			printf("Process Class Type NOT VALID!");
			printf("Process Class Type ( Application 0 or System  1): \n" );
			sys_req(TERMINAL,READ,newPCB -> classType,2);
			isValid = 0;
		}
		//check Priority
		if( 127 <= newPCB -> priority || -128 >= newPCB -> priority ){
		    isValid = 1;
		} else{
			printf("Process Priority (-128 to 127): /n");
			sys_req(TERMINAL,READ,newPCB -> priority,4);
			isValid = 0;
		}
	}
	
	setup_PCB(newPCB);
	insert_PCB(newPCB);
	count++;//Update the number of times the function has run.
}

/** This is a user function in the menu to delete a process it takes the process name as input */
void mpxcmd_delete_PCB(int argc, char *argv[]){
	
}

/** This is a user function in the menu that puts a process in the blocked state it takes the process name as input*/
void mpxcmd_block(int argc, char *argv[]){
	
}

/** This is a user function in the menu that puts a process in the unblocked state it takes the process name as input*/
void mpxcmd_unblock(int argc, char *argv[]){
	
}

/** This is a user function in the menu that puts a process in the suspend state it takes the process name as input*/
void mpxcmd_suspend(int argc, char *argv[]){
	
}

/** This is a user function in the menu that puts a process in the ready state if previously blocked and blocked if previously suspended it takes the process name as input*/
void mpxcmd_resume(int argc, char *argv[]){
	
}

/** This is a user function from the menu it changes the priority of a PCB and takes the name and desired priority as inputs80ij*/
void mpxcmd_setPriority(int argc, char *argv[]){
	
}

/** This is a user command from the menu it is used to show information about a specific PCB*/
void mpxcmd_show_PCB(int argc, char *argv[]){

}

/** This is a user functions that shows name and state of all processes */
void showAll(int argc, char *argv[]){ // Pagination function needs added !!Function still needs work!!
	// redo
}

/** This is a user function that shows all non-suspended processes followed by suspended processes */
void showReady(int argc, char *argv[]){ // Pagination function needs added !!Function still needs work!!
	// redo
}

/** This is a user function that shows all blocked processes followed by non-blocked processes */
void showBlocked(int argc, char *argv[]){ // Pagination function needs added !!Function still needs work!!
	//redo
}	
}



