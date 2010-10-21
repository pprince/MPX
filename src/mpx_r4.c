// Module 4 MPX 

loadProgram(int argc, char *argv[]){ //name,fileName,priority
	
	static int count;
	char name[STRLEN],line[MAX_LINE];
	int size,type,priority,loadAdd,exeAdd;
	PCB *newPCB = allocate_PCB();
		
	if((argc==3)||(127<priority<-128)||((size=sys_check_program())<=0)){
		
		priority=atoi(argv[2]);
		
		if( count == ZERO ){ //If first process allocate queue
			rQueue = (ROOT*) sys_alloc_mem(sizeof(ROOT));
			wsQueue = (ROOT*) sys_alloc_mem(sizeof(ROOT));
		}
		
		setup_PCB(newPCB,argv[0],APPLICATION,SUSPENDED_READY,priority);
		insert_PCB(newPCB);	
		count++;//Update the number of times the function has run.
		
		loadAdd=sys_alloc_mem(size);
		exeAdd=loadAdd+size;
		
		//make sure all registers are properly set
		//load program
	}
	else{
		printf("Wrong arguments entered.");
	}
}

terminateProcess(int argc, char *argv[]){

	if (argc == 1){
		char name[STRLEN];
		PCB *pointer;
		strcpy(name,argv[1]);
		pointer = find_PCB(name);
	
		if ( pointer != NULL){
			remove_PCB(pointer);}

		sys_free_mem()
	
	}
	
	else(){
		printf("Wrong arguments entered.");
		return();
	}
}

dispatch(int argc, char *argv[]){
	//waiting for dispatch from r3
}
