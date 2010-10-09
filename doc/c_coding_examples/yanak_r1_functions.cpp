Nick Yanak
PMOS R1 functions
9/8/10

int bufSize;
long fileSize;
date_rec systemDate;
char directory[80],fileName[40],version["Perpetual Motion OS is currently in version 1.00. Created by Nathanial Clay, 
							Paul Prince, and Nicholas Yanak. PMOS was last modified on 09/13/2010. "];

void Help(){
	printf("The following is a list of commands supported by this version of PMOS. \n
			version-returns the current version of PMOS \n
			setDate(mm,dd,yyyy)-allows you to set the date of PMOS \n
			getDate-returns the current date of PMOS \n
			directory(path)-returns all PMOS related files in the directory you search \n
			terminate-prompts to make sure you want to terminate PMOS and then terminates");
}

void getVersion(){
	printf("PMOS is currently in version %s" version);
}

void getDate(){
	sys_get_date(*systemDate);
	printf("The current date is %d/%d/%d"month,day,year);
}

void setDate(int tmonth, int,tday, int tyear){
	month=tmonth;
	day=tday;
	year=tyear;
	sys_set_date(*systemDate);
}

void displayFiles(char direc[]){

	if(sys_open_dir(direc[])==0){
		for(sys_get_entry(fileName,bufSize,fileSize)!=0){
		}
		sys_close_dir();
	else
		printf("The directory has failed to open.");
}
	
void terminate(){
	printf("Are you sure you want to quit 'y' or 'n'?");
	char temp=sys_req(READ,TERMINAL,temp,1);
		if(temp=='y')
			sys_exit;
}

}
