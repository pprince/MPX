#include "mpx_r2.h"
#include "mpx_supt.h"
#include "mystdlib.h"
#include "mpx_util.h"
#include <string.h>
#include <stdio.h>


ROOT *rQueue; // Remember to reserve memory for these
ROOT *wsQueue;


/** Allocates the memory for a new Process Control Block and returns 
	the pointer to the new PCB location in memory.
*/
PCB *allocate_PCB( void ){
	PCB *newPCB; ///< pointer to the new PCB
	int i;
	MEMDSC *newMemDsc;///< pointer to the Memory Descriptor
	STACKDSC *newStackDsc;///<pointer to the stack descriptor
	unsigned char *stack; ///< pointer to the stack low address
	
	
	// Allocate memory to each of the disctenct parts of the PCB
	newStackDsc = (STACKDSC*) sys_alloc_mem(sizeof(STACKDSC));
	newMemDsc = (MEMDSC*) sys_alloc_mem(sizeof(MEMDSC));
	newPCB = (PCB*) sys_alloc_mem(sizeof(PCB));
	stack = (unsigned char*) sys_alloc_mem(STACKSIZE*sizeof(unsigned char));
	
	if ( stack == NULL || 
		 newStackDsc == NULL || 
		 newMemDsc == NULL || 
		 newPCB == NULL ) return NULL;
	
	//Setup Memory Descriptor with Default Values for Module 2
	newMemDsc -> size = 0;
	newMemDsc -> loadADDR = NULL;
	newMemDsc -> execADDR = NULL;
	
	//Setup the Stack

	memset(stack,0,STACKSIZE*sizeof(unsigned char));//ZERO out Stack to aid in debug....
	newStackDsc -> base = stack; // x86 arch Stacks start at the Higest value 
	newStackDsc -> top  = stack[STACKSIZE-1];// and go to lowest or n - 2 for Word alloc 
	
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
//FIXME: Move to allocate, Create to setup
int setup_PCB( PCB *pointer, char *Name, int classType, int state, int priority ){
	
	char *name = pointer -> name;
	strcpy(name,Name);
	printf("%s \n", pointer -> name);
	pointer -> classType = (signed char) classType;
	pointer -> state = (signed char )  state; 
	pointer -> priority =(signed char) priority;

	return 0;
}
/** This function returns a character string with PCB information formatted. */
char *string_PCB( PCB *pointer){
	char line_buf[MAX_LINE];
	char *name = pointer -> name;
	signed char *classType = pointer -> classType;
	signed char *stateType = pointer -> state;
	signed char *priority = pointer -> priority;
	char class[60];
	char state[60];
	
	if( classType = APPLICATION ) strcpy( class, "Application");
	if( classType = SYSTEM ) strcpy( class, "System" );
	
	if( stateType = RUNNING ) strcpy(state,"Running");
	if( stateType = READY ) strcpy( state ,"Ready" );
	if( stateType = BLOCKED ) strcpy( state ,"Blocked");
	if( stateType = SUSPENDED_READY ) strcpy(state ,"Suspended Ready");
	if ( stateType = SUSPENDED_BLOCKED ) strcpy( state,"Suspended Blocked" ) ;
	
	
    sprintf(&line_buf,"Name: %s  Class: %s State: %s Priority: %d ", name, class, state,priority); 
	
	return line_buf;
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
		node-> left = incr;
		node-> right = NULL;
		incr->right = node;
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
	
	if( queueROOT -> node = NULL ){ // if this is the first element ever in the queque
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
	 queueROOT -> count += 1; // increase count by one as the size of the queque has grown by one
	 
	 return;

}

PCB *find_PCB( char *name){
	ELEM *incr;
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
	if ( incr -> process != NULL && incr != NULL ){
		return incr->process;
	}else{
		return NULL;
	}

}
void remove_PCB( PCB *process ){
	ROOT *queue;
	ELEM *incr;
	ELEM *temp1;
	ELEM *temp2;
	
	if ( process -> state == READY || process -> state == RUNNING ){
		queue = rQueue;
	}
	if( process -> state == BLOCKED || 
		process -> state == SUSPENDED_READY || 
		process -> state == SUSPENDED_BLOCKED ){
		queue = wsQueue;
	}
	
	incr = queue-> node; //set node to the first node in the queque
	while ( (incr -> process != process ) && incr != NULL ){ // find the same process
			incr = incr -> right; // progrees to the right 
	}
	/* There are three cases to check for head, tail, and middle*/
	
	/*head case*/
	if( incr -> left == NULL && incr->right != NULL ){ 
		incr = incr -> right;
		incr -> left  = NULL;
		queue -> node = incr -> right; //set queueROOT to new head
		queue ->count -=1;
	}
	
	/*tail case*/
	if( incr -> left != NULL && incr->right == NULL ){
		incr = incr-> left;
		incr -> right  = NULL;
		queue -> count -=1;
	}
	
	/*middle case*/
	if( incr -> left != NULL && incr->right != NULL){
		temp1 = incr -> left;
		temp1 -> right = incr -> right;
		temp2 = incr -> right;
		temp2 -> left = incr -> left;
		queue -> count -=1;
	}
	//Deallocate mem
	free_PCB(process);
	sys_free_mem(incr);
	
	return;
	}


void mpxcmd_create_PCB(int argc, char *argv[]){
	
	char name[STRLEN];
	char line[MAX_LINE];
	int type;
	int priority;
	
	static int count = ZERO;
	char *lp = &line;
	
	
	PCB *newPCB = allocate_PCB();
	
	if( count == ZERO ){
		rQueue = (ROOT*) sys_alloc_mem(sizeof(ROOT));
		wsQueue = (ROOT*) sys_alloc_mem(sizeof(ROOT));
	}
	
	
	printf("Process Name: \n");
	mpx_readline(name, STRLEN);
	printf("Process Class Type ( Application 0 or System  1): \n" );
	type= mpxprompt_int();
	printf("Process Priority (-128 to 127): \n");
	priority = mpxprompt_int();
	
	
	
	setup_PCB(newPCB,&name,type,READY,priority);
	//lp = string_PCB(newPCB);
	//printf("%s",lp);
	mpxprompt_anykey();
	insert_PCB(newPCB);
	count++;//Update the number of times the function has run.
}
PCB *copy_PCB(PCB *pointer){ 
		PCB *tempPCB = allocate_PCB();
		memcpy(tempPCB,pointer,sizeof(PCB));
		memcpy(tempPCB -> memdsc, pointer -> memdsc , sizeof(MEMDSC));
		memcpy(tempPCB -> stackdsc ,pointer -> stackdsc, sizeof(STACKDSC));
	return tempPCB;
}
/** This is a user function in the menu to delete a process it takes the process name as input */
void mpxcmd_delete_PCB(int argc, char *argv[]){
	char name[STRLEN];
	int buffs;
	PCB *pointer;
	buffs = STRLEN;
	printf("Name Of Process to Delete: \n");
	sys_req(READ,TERMINAL,name,&buffs);
	
	pointer = find_PCB(name);
	if ( pointer != NULL){
		remove_PCB(pointer);
	}else{
		printf("Process Name not found!");
		return;
	}
	
}

/** This is a user function in the menu that puts a process in the blocked state it takes the process name as input*/
void mpxcmd_block(int argc, char *argv[]){
	char name[STRLEN];
	PCB *pointer;
	PCB *tempPCB;
	int buffs = STRLEN;
	printf("Name Of Process to block: \n");
	sys_req(READ,TERMINAL,name,&buffs);
	
	pointer = find_PCB(name);
	if ( pointer != NULL){
		tempPCB = copy_PCB(pointer);
		remove_PCB(pointer);
		if( tempPCB -> state > 0 ) tempPCB -> state = BLOCKED;
		if( tempPCB -> state < 0 && tempPCB -> state == SUSPENDED_READY ) tempPCB -> state = SUSPENDED_BLOCKED;
		insert_PCB(tempPCB);
	}else{
		printf("Process Name not found!");
		return;
	}
}

/** This is a user function in the menu that puts a process in the unblocked state it takes the process name as input*/
void mpxcmd_unblock(int argc, char *argv[]){
	char name[STRLEN];
	PCB *pointer;
	PCB *tempPCB;
	int buffs = STRLEN;
	printf("Name Of Process to unblock: \n");
	sys_req(READ,TERMINAL,name,&buffs);
	
	pointer = find_PCB(name);
	if ( pointer != NULL){
		tempPCB = copy_PCB(pointer);
		remove_PCB(pointer);
		if( tempPCB -> state == BLOCKED ) tempPCB -> state = READY;
		if( tempPCB -> state == SUSPENDED_BLOCKED ) tempPCB -> state = SUSPENDED_READY;
		insert_PCB(tempPCB);
	}else{
		printf("Process Name not found!");
		return;
	}
}

/** This is a user function in the menu that puts a process in the suspend state it takes the process name as input*/
void mpxcmd_suspend(int argc, char *argv[]){
	char name[STRLEN];
	PCB *pointer;
	PCB *tempPCB;
	int buffs = STRLEN;
	printf("Name Of Process to suspend: \n");
	sys_req(READ,TERMINAL,name,&buffs);
	
	pointer = find_PCB(name);
	if ( pointer != NULL){
		tempPCB = copy_PCB(pointer);
		remove_PCB(pointer);
		if( tempPCB -> state == READY || tempPCB -> state == RUNNING ) tempPCB -> state = SUSPENDED_READY;
		if( tempPCB -> state == BLOCKED ) tempPCB -> state = SUSPENDED_BLOCKED;
		insert_PCB(tempPCB);
	}else{
		printf("Process Name not found!");
		return;
	}
}

/** This is a user function in the menu that puts a process in the ready state if previously blocked and blocked if previously suspended it takes the process name as input*/
void mpxcmd_resume(int argc, char *argv[]){
	char name[STRLEN];
	PCB *pointer;
	PCB *tempPCB;
	int buffs = STRLEN;
	printf("Name Of Process to resume: \n");
	sys_req(READ,TERMINAL,name,&buffs);
	
	pointer = find_PCB(name);
	if ( pointer != NULL){
		tempPCB = copy_PCB(pointer);
		remove_PCB(pointer);
		if( tempPCB -> state == SUSPENDED_READY ) tempPCB -> state = READY;
		if( tempPCB -> state == SUSPENDED_BLOCKED ) tempPCB -> state = BLOCKED;
		insert_PCB(tempPCB);
	}else{
		printf("Process Name not found!");
		return;
	}
}

/** This is a user function from the menu it changes the priority of a PCB and takes the name and desired priority as inputs80ij*/
void mpxcmd_setPriority(int argc, char *argv[]){
	char name[STRLEN];
	PCB *pointer;
	PCB *tempPCB;
	int buffs = STRLEN;
	printf("Name Of Process to set Priority: \n");
	sys_req(READ,TERMINAL,name,&buffs);
	
	pointer = find_PCB(name);
	if ( pointer != NULL){
		tempPCB = copy_PCB(pointer);
		remove_PCB(pointer);
		if( tempPCB -> state == SUSPENDED_READY ) tempPCB -> state = READY;
		if( tempPCB -> state == SUSPENDED_BLOCKED ) tempPCB -> state = BLOCKED;
		insert_PCB(tempPCB);
	}else{
		printf("Process Name not found!");
		return;
	}
}


/** This is a user command from the menu it is used to show information about a specific PCB*/
void mpxcmd_show_PCB(int argc, char *argv[]){
	char name[STRLEN];
	PCB *pointer;
	char class[30];
	char state[45];
	int buffs = STRLEN;
	printf("Name Of Process to show: \n");
	sys_req(READ,TERMINAL,name,&buffs);
	
	pointer = find_PCB(name);
	if ( pointer != NULL){
		
		
		if ( pointer -> classType  == APPLICATION )
			strcpy( class, "Application" );
		if (  pointer -> classType == SYSTEM )
			strcpy ( class, "System" );
		
		
		if ( pointer -> state == RUNNING ) 
			strcpy( state , "Running" );
		if ( pointer -> state == READY ) 
			strcpy( state, "Ready" );
		if ( pointer -> state == BLOCKED ) 
			strcpy( state, "Blocked" );
		if ( pointer -> state == SUSPENDED_READY ) 
			strcpy( state ,"Suspened Ready" );
		if ( pointer -> state == SUSPENDED_BLOCKED ) 
			strcpy( state , "Suspened Blocked" ) ;
		
		printf("Name: %s \n Class: %s \n Priority: %d \n State:  %s \n", pointer -> name, class, pointer -> priority, state); 
	}else{
		printf("Process Name not found!");
		return;
	}
}

/** This is a user functions that shows name and state of all processes */
void mpxcmd_showAll_PCB(int argc, char *argv[]){ // Pagination function needs added !!Function still needs work!!
	ELEM *incr;
	PCB *pointer;
	char class[30];
	char state[45];
	incr = rQueue -> node;//set node to the first node in the queque
	while( incr -> right != NULL ){
		
		pointer = incr -> process;
		
		if ( pointer -> classType  == APPLICATION )
			strcpy( class, "Application" );
		if (  pointer -> classType == SYSTEM )
			strcpy ( class, "System" );
		
		
		if ( pointer -> state == RUNNING ) 
			strcpy( state , "Running" );
		if ( pointer -> state == READY ) 
			strcpy( state, "Ready" );
		if ( pointer -> state == BLOCKED ) 
			strcpy( state, "Blocked" );
		if ( pointer -> state == SUSPENDED_READY ) 
			strcpy( state ,"Suspened Ready" );
		if ( pointer -> state == SUSPENDED_BLOCKED ) 
			strcpy( state , "Suspened Blocked" ) ;
		
		
		printf("Name: %s \n Class: %s \n Priority: %i \n State:  %s \n", pointer -> name, class, pointer -> priority, state); 
		incr = incr -> right; // progress forward to the right of the queque
	}
	incr = wsQueue -> node;
	while( incr -> right != NULL ){
		
		pointer = incr -> process;
	
		if ( pointer -> classType  == APPLICATION )
			strcpy( class, "Application" );
		if (  pointer -> classType == SYSTEM )
			strcpy ( class, "System" );
		
		
		if ( pointer -> state == RUNNING ) 
			strcpy( state , "Running" );
		if ( pointer -> state == READY ) 
			strcpy( state, "Ready" );
		if ( pointer -> state == BLOCKED ) 
			strcpy( state, "Blocked" );
		if ( pointer -> state == SUSPENDED_READY ) 
			strcpy( state ,"Suspened Ready" );
		if ( pointer -> state == SUSPENDED_BLOCKED ) 
			strcpy( state , "Suspened Blocked" ) ;
		
		printf("Name: %s \n Class: %s \n Priority: %i \n State:  %s \n", pointer -> name, class, pointer -> priority, state); 
		incr = incr -> right; // progress forward to the right of the queque
	}
}

/** This is a user function that shows all non-suspended processes followed by suspended processes */
void mpxcmd_showReady_PCB(int argc, char *argv[]){ // Pagination function needs added !!Function still needs work!!
	ELEM *incr;
	PCB *pointer;
	char class[30];
	char state[45];
	incr = rQueue -> node;//set node to the first node in the queque
	
	while( incr -> right != NULL ){
		
		pointer = incr -> process;
		if ( pointer -> state != READY) break;
		
		if ( pointer -> classType  == APPLICATION )
			strcpy( class, "Application" );
		if (  pointer -> classType == SYSTEM )
			strcpy ( class, "System" );
		
		
		if ( pointer -> state == RUNNING ) 
			strcpy( state , "Running" );
		if ( pointer -> state == READY ) 
			strcpy( state, "Ready" );
		if ( pointer -> state == BLOCKED ) 
			strcpy( state, "Blocked" );
		if ( pointer -> state == SUSPENDED_READY ) 
			strcpy( state ,"Suspened Ready" );
		if ( pointer -> state == SUSPENDED_BLOCKED ) 
			strcpy( state , "Suspened Blocked" ) ;
		
		printf("Name: %s \n Class: %s \n Priority: %i \n State:  %s \n", pointer -> name, class, pointer -> priority, state); 
		incr = incr -> right; // progress forward to the right of the queque
	}
	while( incr -> right != NULL ){
		pointer = incr -> process;
		if ( pointer -> state != SUSPENDED_READY) break;
		
		if ( pointer -> classType  == APPLICATION )
			strcpy( class, "Application" );
		if (  pointer -> classType == SYSTEM )
			strcpy ( class, "System" );
		
		
		if ( pointer -> state == RUNNING ) 
			strcpy( state , "Running" );
		if ( pointer -> state == READY ) 
			strcpy( state, "Ready" );
		if ( pointer -> state == BLOCKED ) 
			strcpy( state, "Blocked" );
		if ( pointer -> state == SUSPENDED_READY ) 
			strcpy( state ,"Suspened Ready" );
		if ( pointer -> state == SUSPENDED_BLOCKED ) 
			strcpy( state , "Suspened Blocked" ) ;
		
		printf("Name: %s \n Class: %s \n Priority: %i \n State:  %s \n", pointer -> name, class, pointer -> priority, state); 
		incr = incr -> right; // progress forward to the right of the queque
	}
}

/** This is a user function that shows all blocked processes followed by non-blocked processes */
void mpxcmd_showBlocked_PCB(int argc, char *argv[]){ // Pagination function needs added !!Function still needs work!!
	ELEM *incr;
	char class[30];
	char state[45];
	PCB *pointer;
	incr = rQueue -> node;//set node to the first node in the queque
	while( incr -> right != NULL ){
		
		pointer = incr -> process;
		if ( pointer -> state != BLOCKED) break;
		
		
		if ( pointer -> classType  == APPLICATION )
			strcpy( class, "Application" );
		if (  pointer -> classType == SYSTEM )
			strcpy ( class, "System" );
		
		
		if ( pointer -> state == RUNNING ) 
			strcpy( state , "Running" );
		if ( pointer -> state == READY ) 
			strcpy( state, "Ready" );
		if ( pointer -> state == BLOCKED ) 
			strcpy( state, "Blocked" );
		if ( pointer -> state == SUSPENDED_READY ) 
			strcpy( state ,"Suspened Ready" );
		if ( pointer -> state == SUSPENDED_BLOCKED ) 
			strcpy( state , "Suspened Blocked" ) ;
		
		
		printf("Name: %s \n Class: %s \n Priority: %i \n State:  %s \n", pointer -> name, class, pointer -> priority, state); 
		incr = incr -> right; // progress forward to the right of the queque
	}
	while( incr -> right != NULL ){
		pointer = incr -> process;
		if ( pointer -> state != SUSPENDED_BLOCKED) break;
		
		if ( pointer -> classType  == APPLICATION )
			strcpy( class, "Application" );
		if (  pointer -> classType == SYSTEM )
			strcpy ( class, "System" );
		
		
		if ( pointer -> state == RUNNING ) 
			strcpy( state , "Running" );
		if ( pointer -> state == READY ) 
			strcpy( state, "Ready" );
		if ( pointer -> state == BLOCKED ) 
			strcpy( state, "Blocked" );
		if ( pointer -> state == SUSPENDED_READY ) 
			strcpy( state ,"Suspened Ready" );
		if ( pointer -> state == SUSPENDED_BLOCKED ) 
			strcpy( state , "Suspened Blocked" ) ;
		
		printf("Name: %s \n Class: %s \n Priority: %i \n State:  %s \n", pointer -> name, class, pointer -> priority, state); 
		incr = incr -> right; // progress forward to the right of the queque
	}
}	

