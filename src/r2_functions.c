//Functions for R2
// all references of insert,find, and delete PCB need to be edited once the functions are made

/**  This funtion is a user function in the menu to create a process.
 It takes as arguments the name(first 16 chars),class(0 for system and 1 for application),and priority(-128 to 127)  */
void createPCB(char name[STRLEN],signed char type,signed char priority){
	if((name!=null)&&(type==0||type==1)&&(-128<=priority<=127)){ ///< checks too see if data is valid
		PCB tempPCB=mpx_r2.setup_PCB(mpx_r2.allocate_PCB(),name,type); ///< creates and sets the data for the process
		setPriority(name,priority); ///< sets the priority
		tempPCB->state=0x04; ///< sets the state
		mpx_r2.insert_PCB(tempPCB); ///< inserts the process into the apropriate queue
	}
	else
		printf("Invalid data entered please try again");
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
	}
	else{
		tempPCB->state=0x08; ///< sets the state to blocked
	}
}

/** This is a user function in the menu that puts a process in the unblocked state it takes the process name as input*/
void unblock(char name[STRLEN]){
	PCB tempPCB=mpx_r2.find_PCB(name); ///< finds the pcb
	
	if(tempPCB==null){ ///< checks to make sure the PCB was found
		printf("The process could not be found");
	}
	else{
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
void showAll(){  // Pagination function  needs added !!Function still needs work!!
	PCB tempPCB=mpx_r2.getHead(); 
	while(tempPCB.next!=null){ ///< cycles through each PCB in the linked list and prints out name and state
		PCB tempPCB=tempPCB.next;
		printf("%s %s \n",name,tempPCB.state);
	}
}

/** This is a user function that shows all non-suspended processes followed by suspended processes */
void showReady(){  // Pagination function  needs added !!Function still needs work!!
	PCB tempPCB=mpx_r2.getHead();
	while(tempPCB.next!=null){
		PCB tempPCB=tempPCB.next;   
		if(tempPCB.state==ready)
			printf("%s,%s,%s \n",name,priority,status);
		}
	}
}

/** This is a user function that shows all blocked processes followed by non-blocked processes */
void showBlocked(){ // Pagination function  needs added !!Function still needs work!!
	PCB tempPCB=mpx_r2.getHead();
	while(tempPCB.next!=null){
		PCB tempPCB=tempPCB.next;   
		if(tempPCB.state==ready)
			printf("%s,%s,%s \n",name,priority,status);
		}
	}  
}


