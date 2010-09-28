#include "mpx_r2.h"
#include "mpx_supt.h"
#include "mpx_supt.c"
#include "mystdlib.h"
#include <string.h>
#include <stdio.h>

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

void setup_PCB( PCB *pointer, char *name,int classType){
	int i;///< counter varable for loop 
	unsigned char *stack;///< pointer to what will become the bottom of the stack 
	char *nPtr = pointer -> name; ///< pointer the name string in the PCB  
	//Allocate the stack
	stack = (unsigned char*) sys_mem_alloc(STACKSIZE);
	
	//copy name into the PCB
	//if ( findPCB( name ) == NULL) {
		strncpy(nPtr, name,STRLEN);
	//}else{
	//	printf("Process Name NOT VALID");
	//}	
		
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
	pointer -> stackdsc -> base = stack; // x86 arch Stacks start at the Higest value 
	pointer -> stackdsc -> top  = stack[STACKSIZE-1];// and go to lowest or n - 2 for Word alloc 
	
}

void insert_PCB(PCB *PCBpointer/*< pointer to a PCB to insert*/ , ROOT *quequeROOT /*< points to the head node of the queque */ , int ORD /*< code for order to insert PCB Prioroity and FIFO*/){
   
   switch(ORD){
		case PORDR:
			insert_PORDR(PCBpointer,quequeROOT);
			break;
		case FIFO:
			insert_FIFO(PCBpointer,quequeROOT);
			break;
		default:
			printf("ORDER not Valid");
			break;
		};
}

void insert_PORDR( PCB *PCBpointer, ROOT *quequeROOT ){ //FIXME: NO ERROR CHECKING
	ELEM *node; // declare node of type element
	ELEM *incr;
	node = sys_alloc_mem( sizeof(ELEM)); // allocate Memory for node
	node -> process = PCBpointer;// add the PCB to the node
	
	if( quequeROOT -> node = NULL ){ // if this is the first element ever in the queque
		node -> left = NULL;
		node -> right = NULL;
		quequeROOT -> node = node; // Set the first element in the queque to node of Type Element
		quequeROOT -> count += 1; // increase count by one
		return; //exit out first node is in queque. 
	}
	incr = quequeROOT -> node; //set node to the first node in the queque
	while ( incr -> process -> priority <= node -> process -> priority ){ // Process with the lowest priority goes first 
			incr = incr -> right; // progrees to the right 
	}
	/* There are three cases to check for head, tail, and middle*/
	
	/*head case*/
	if( incr -> left == NULL && incr->right != NULL ){ 
		node->left = NULL;
		node->right = incr;
		incr->left = node;
		quequeROOT -> node = node; //set quequeROOT to new head
		quequeROOT ->count +=1;
	}
	
	/*tail case*/
	if( incr -> left != NULL && incr->right == NULL ){
		node-> left = &incr;
		node-> right = NULL;
		incr->right = &node;
		quequeROOT->count +=1;
	}
	
	/*middle case*/
	if( incr -> left != NULL && incr->right != NULL){
		node->right = incr->right;
		incr->right = node;
		node->left = incr;
		quequeROOT->count +=1;
	}
	return;
}
/** In this function we grow the queque to the right no matter of the Priority of the PCB.*/ 
void insert_FIFO( PCB *PCBpointer, ROOT *quequeROOT){ //FIXME: NO ERROR HANDLING
	ELEM *node; // declare node of type element
	ELEM *incr;
	node = sys_alloc_mem( sizeof(ELEM)); // allocate Memory for node
	node -> process = PCBpointer;// add the PCB to the node
	
	if( quequeROOT -> node = NULL ){ // if this is the first element ever in the queque
		node -> left = NULL; // set the link left to null
		node -> right = NULL;// set the link right to null
		quequeROOT -> node = node; // Set the first element in the queque to node of Type Element
		quequeROOT -> count += 1; // increase count by one
		return; //exit out first node is in queque. 
	}
	
	
	/* INSERT INTO THE QUEQUE IN FIFO ORDER*/
	incr = quequeROOT -> node; //set node to the first node in the queque
	while( incr -> right != NULL ){
		incr = incr -> right; // progress forward to the right of the queque
	}
	 node -> left = incr; //set left to previous node
	 node -> right = NULL; // set right to null 
	 quequeROOT -> count += 1; // increase count by one as the size of the queque has grown by one
	 
	 return;

}



