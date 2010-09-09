#include "mpx_r1.h"
#include "mpx_supt.h"

/* Symbolic Constants */
#define DONE 0

/* Strings */
char *welcome_message_str = "Welcome to Perpetual Motion Squad's Operating System.\n";
char *prompt = "#>";

int displayPrompt(void){
	
	// go to line one
	char buf[80];
	int size;
	int err;

	buf[0] = 0;
	buf[1] = 1;
	size = 2;

	err = sys_req(GOTOXY, TERMINAL, buf, &size);

	if (err != OK ){
		return err;
	}
	size = 2;

	err = sys_req(WRITE, TERMINAL, &prompt, &size);

	if (err != OK ){
		return err;
	}
	
	// if we get here good to go
	return OK;
}

void mpx_cls (void) {
	/* fixme: add error catching */
	int err = sys_req(CLEAR, TERMINAL, NULL, 0);
	if ( err != OK ){
		// do something here
}
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

	if (err != OK ){
		return err;
	}


	//Print Welcome
	size = 53;  //??? somehow err is getting  set to 53?

	err = sys_req(WRITE, TERMINAL, &welcome_message_str, &size);

	if (err != OK ){
		return err;
	}

	// if we reach here every thing went ok
	return OK;
} 

int r1( void ){
	int err;
	int flag;

	err = printWelcome();

	if ( err != OK ){
		return err;
	}
	
	err = displayPrompt( );
	
	if ( err != OK ){
		return err;
	}
	// flag = 0;
	// while( flag != DONE ){
		
		// display prompt
		// accept command
		// analize command
		// execute command

	// }
	
	return OK;
}

