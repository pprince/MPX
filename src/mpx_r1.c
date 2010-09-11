#include "mpx_r1.h"
#include "mpx_supt.h"
#include <string.h>

/* Symbolic Constants */
#define DONE 0
#define COMMLEN 80
#define ARGS 30 
#define MAXCOMM 10
/* Strings */
char welcome_message_str[] = "Welcome to Perpetual Motion Squad's Operating System.\n";
char prompt[] = "#>";
char input[COMMLEN];
char *arguments[ARGS];
//char command[ARGS];

/* Flag */
enum trip{
	true, /* true is 0 false is one */
	false
}isDone;


enum trip isDone = false;
// Command Execution Data Type

typedef struct commandExe{
	char *commandName[];
	int (*pt2commandfunct)(char *args); 
	};

struct commandExe commandexec[MAXCOMM];
/* Notes: */	
	/* 
This is the proper way to use sys_req WRITE:
	size = 2;
	err = sys_req(WRITE, TERMINAL, prompt, &size);
	if ( err != size ) retrun err;
	*/
int analizeCommand( char *input, char *args ){
	tokenize( input, args );
	return OK;
}
int tokenize( char *input,char *args[]){
	int count = 0;
	char *tokenptr;
	tokenptr = strtok(input," ");
	while ( tokenptr != NULL ){
		args[count] = tokenptr;
		tokenptr = strtok(NULL," ");
		count++;
	}
	return OK;
}
int acceptCommand(char *comm){
   int err;
   int size = COMMLEN;
   err = sys_req(READ, TERMINAL, comm, &size);
   
   if( err < 0  && err < size) return err;
   
   return OK;
}
int displayPrompt(void){
	
	// go to line one
	char buf[80];
	int size;
	int err;

	buf[0] = 0;
	buf[1] = 1;
	size = 2;

	err = sys_req(GOTOXY, TERMINAL, buf, &size);

	if ( err != OK ) return err;
	
	size = 2;
	err = sys_req(WRITE, TERMINAL, prompt, &size);
	
	if( err != size) return err;
	
	// if we get here good to go
	return OK;
}

int mpx_cls (void) {
	/* fixme: add error catching */
	int err = sys_req(CLEAR, TERMINAL, NULL, 0);
	
	if ( err != OK ) return err;
	
	return OK;
}
int printWelcome( void ){

	// Initialize cursor at home
	char buf[80];
	int size;
	int err;

	// clear screen
	mpx_cls();

	buf[0] = 0;
	buf[1] = 0;
	size = 2;

	err = sys_req(GOTOXY, TERMINAL, buf, &size);

	if ( err != OK ) return err;


	//Print Welcome
	
	size = 53;
	err = sys_req(WRITE, TERMINAL, welcome_message_str, &size);
	
	if( err != size) return err;
	
	// if we reach here every thing went ok
	return OK;
} 

int r1( void ){
	int err;

	err = printWelcome();

	if ( err != OK ) return err;

	while( isDone != DONE ){
		
		//display prompt
		err = displayPrompt( );
		if ( err != OK ) return err;
		// accept command
		err = acceptCommand(input);
		if ( err != OK ) return err;
		// analize command
		err = analizeCommand(input, arguments);
		if ( err != OK ) return err;
		// execute command
		err =  executeCommand(arguments);
	}
	
	return OK;
}
int executeCommand(arguments){
	return OK;
}