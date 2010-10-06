#include "mpx_r2.h"
#include "mpx_supt.h"
#include "mpx_supt.c"
#include "mystdlib.h"
#include <string.h>
#include <stdio.h>


ROOT rQueue; // Remember to reserve memory for these
ROOT wsQueue;


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
	
	copy name into the PCB
	if ( find_PCB( name,rQueue ) == NULL ) {
		strncpy(nPtr, name,STRLEN);
	}else{
		if ( find_PCB( name,wsQueue) == NULL ){
			strncpy(nPtr, name,STRLEN);
		else{
		printf("Process Name NOT VALID");
	 	return;
		}
	}	
		
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

void insert_PCB(PCB *PCBpointer/*< pointer to a PCB to insert*/ , ROOT *queueROOT /*< points to the head node of the queque */ , int ORD /*< code for order to insert PCB Prioroity and FIFO*/){
   
   switch(ORD){
		case PORDR:
			insert_PORDR(PCBpointer,queueROOT);
			break;
		case FIFO:
			insert_FIFO(PCBpointer,queueROOT);
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

PCB *find_pcb( char *name, ROOT *Queue){
	incr =  Queue -> node; //set node to the first node in the queque
	while ( strcmp(name,incr -> process -> name ) != 0 && incr != NULL){ // Process with the lowest priority goes first 
			incr= incr -> right; // progrees to the right 
	}
	
	return incr;

}

void createPCB(char name[STRLEN],signed char type,signed char priority){
	if((name!=null)&&(type==0||type==1)&&(-128<=priority<=127)){ ///< checks too see if data is valid
		PCB tempPCB=mpx_r2.setup_PCB(mpx_r2.allocate_PCB(),name,type); ///< creates and sets the data for the process
		setPriority(name,priority); ///< sets the priority
		tempPCB->state=0x04; ///< sets the state
		insert_PCB(tempPCB); ///< inserts the process into the apropriate queue
	}else{
		printf("Invalid data entered please try again");
	}
}

/** This is a user function in the menu to delete a process it takes the process name as input */
void deletePCB(char name[STRLEN]){
	int valid=mpx_r2.remove_PCB(name); ///<removes pcb and returns an int
	if((valid!=0)&&(name!=null)){ ///< checks to make sure it was removed
	printf("The process could not be found");
	}
}

/** This is a user function in the menu that puts a process in the blocked state it takes the process name as input*/
void block(char name[STRLEN]){
	PCB tempPCB=mpx_r2.find_PCB(name); ///< finds the pcb

	if(tempPCB==null){ ///< checks to make sure the PCB was found
		printf("The process could not be found");
	}else{
	tempPCB->state=0x08; ///< sets the state to blocked
}
}

/** This is a user function in the menu that puts a process in the unblocked state it takes the process name as input*/
void unblock(char name[STRLEN]){
	PCB tempPCB=mpx_r2.find_PCB(name); ///< finds the pcb

	if(tempPCB==null){ ///< checks to make sure the PCB was found
		printf("The process could not be found");
		}else{
		tempPCB->state=0x04; ///< sets the state to ready
	}
}

/** This is a user function in the menu that puts a process in the suspend state it takes the process name as input*/
void suspend(char name[STRLEN]){
	PCB tempPCB=mpx_r2.find_PCB(name); ///< finds the pcb

if(tempPCB==null){ ///< checks to make sure the PCB was found
	printf("The process could not be found");
}
else{
		if(tempPCB.state==0x04){ ///< if ready makes it suspended ready
		tempPCB->state=0xFB;
		}
		else if(tempPCB.state==0x08){ ///< if blocked makes it ready blocked
		tempPCB->state=0xF7;
		}
	}
}

/** This is a user function in the menu that puts a process in the ready state if previously blocked and blocked if previously suspended it takes the process name as input*/
void resume(char name[STRLEN]){
PCB tempPCB=mpx_r2.find_PCB(name); ///< finds the pcb

if(tempPCB==null){ ///< checks to make sure the PCB was found
printf("The process could not be found");
}
else{
if(tempPCB.state==0x08){ ///< if blocked makes it ready
tempPCB->state=0x04;
}
else if(tempPCB.state==0xFF){ ///< if suspended makes it blocked
tempPCB->state=0x0;
}
}
}

/** This is a user function from the menu it changes the priority of a PCB and takes the name and desired priority as inputs80ij*/
void setPriority(char name [STRLEN],signed char priority){
PCB tempPCB=mpx_r2.find_PCB(name);

if((tempPCB==null)||(127<=priority<=-128)){ ///< checks for validity
printf("The process could not be found");
}
else{
mpx_r2.remove(tempPCB);
mpx_r2.insert(tempPCB);
}

}

/** This is a user command from the menu it is used to show information about a specific PCB*/
void showPCB(char name [STRLEN]){
PCB tempPCB=mpx_r2.find_PCB(name);
if(tempPCB==null){ ///< checks to make sure the PCB was found
printf("The process could not be found");
}
else{
printf("Process name: %s \n State of process: %s \n",name,tempPCB.state);
}
}

/** This is a user functions that shows name and state of all processes */
void showAll(){ // Pagination function needs added !!Function still needs work!!
PCB tempPCB=mpx_r2.getHead();
while(tempPCB.next!=null){ ///< cycles through each PCB in the linked list and prints out name and state
PCB tempPCB=tempPCB.next;
printf("%s %s \n",name,tempPCB.state);
}
}

/** This is a user function that shows all non-suspended processes followed by suspended processes */
void showReady(){ // Pagination function needs added !!Function still needs work!!
PCB tempPCB=mpx_r2.getHead();
while(tempPCB.next!=null){
PCB tempPCB=tempPCB.next;
if(tempPCB.state==ready)
printf("%s,%s,%s \n",name,priority,status);
}
}
}

/** This is a user function that shows all blocked processes followed by non-blocked processes */
void showBlocked(){ // Pagination function needs added !!Function still needs work!!
PCB tempPCB=mpx_r2.getHead();
while(tempPCB.next!=null){
PCB tempPCB=tempPCB.next;
if(tempPCB.state==ready)
printf("%s,%s,%s \n",name,priority,status);
}
}
}



