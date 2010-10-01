//Functions for R2
// all references of insert,find, and delete PCB need to be edited once the functions are made

/**  This funtion is a user function in the menu to create a process.
 It takes as arguments the name(first 16 chars),class(0 for system and 1 for application),and priority(-128 to 127)  */
void createPCB(char name[STRLEN],signed char type,signed char priority){
	if((name!=null)&&(type==0||type==1)&&(-128<=priority<=127)){ ///< checks too see if data is valid
		mpx_r2.setup_PCB(mpx_r2.allocate_PCB(),name,type); ///< creates and sets the data for the process
		setPriority(name,priority);
		mpx_r2.insert_PCB(); ///< inserts the process into the apropriate queue
	}
	else
		printf("Invalid data entered please try again");
}

/** This is a user function in the menu to delete a process it takes the process name as input */
void deletePCB(char name[STRLEN]){
	int valid=mpx_r2.remove_PCB(name);
	if((valid!=0)&&(name!=null))
		printf("The process could not be found");
}

/** This is a user function in the menu that puts a process in the blocked state it takes the process name as input*/
void block(char name[STRLEN]){
	int valid=mpx_r2.remove_PCB(name);
	if((valid!=0)&&(name!=null))
		printf("The process could not be found");
		
		int valid=mpx_r2.insert_PCB();
	if((valid!=0)&&(name!=null))
		printf("The process could not be found");	
}

/** This is a user function in the menu that puts a process in the unblocked state it takes the process name as input*/
void unblock(char name[STRLEN]){
		int valid=mpx_r2.remove_PCB(name);
	if((valid!=0)&&(name!=null))
		printf("The process could not be found");
		
		int valid=mpx_r2.insert_PCB();
	if((valid!=0)&&(name!=null))
		printf("The process could not be found");	
}

/** This is a user function in the menu that puts a process in the suspend state it takes the process name as input*/
void suspend(char name[STRLEN]){
			PCB tempPCB=mpx_r2.find_PCB(name);
	if((tempPCB==null)||(name==null)){
		printf("The process could not be found");
	}
	else{
		tempPCB->state=suspend; // make this an int
	}
}

/** This is a user function in the menu that puts a process in the ready state if previously blocked and blocked if previously suspended it takes the process name as input*/
void resume(char name[STRLEN]){
	PCB tempPCB=mpx_r2.find_PCB(name);
	if(tempPCB.state==blocked){
		tempPCB->state=ready; // make this an int
	}
	else if(tempPCB.state==suspend){
		tempPCB->state=blocked; // make this an int
	}
	else((tempPCB==null)||(name==null)){
		printf("The process could not be found");
	}
}

void setPriority(char name [STRLEN],signed char priority){
		PCB tempPCB=mpx_r2.find_PCB(name);
		if((tempPCB==null)||(name==null)){
		printf("The process could not be found");
	}

}

void showPCB(char name [STRLEN]){
	PCB tempPCB=mpx_r2.find_PCB(name);
	printf("Process name: %s \n State of process: %s \n",);}

void showAll(){
	PCB tempPCB=mpx_r2.getHead();
	while(tempPCB.next!=null){
		PCB tempPCB=tempPCB.next;
		printf("%s,%s \n",);
	}
}
void showReady(){
	PCB tempPCB=mpx_r2.getHead();
	while(tempPCB.next!=null){
		PCB tempPCB=tempPCB.next;
		if(tempPCB.state==ready)
			printf("%s,%s,%s \n",name,priority,status);
	}
}

void showBlocked(){
}


