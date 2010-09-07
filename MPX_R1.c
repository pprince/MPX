#include <MPX_SUPT.h>
#include <MPX_SUPT.c>

#define char OPEN[] = "Welcome to Perpetual Motion Squad's Operating System.\0"; 
#define int DONE = 0;


int printWelcome( void ){

// Initialize cursor at home
char buf[80];
int size;
int err;

buf[0] = 0;
buf[1] = 0;
size = 2;

err = sys_req(GOTOXY, TERMINAL, buf, &size);

if (err ! = OK ){
	return err;
}


//Print Welcome
size = 53;

err = sys_req(WRITE, TERMINAL, &OPEN, size);

if (err ! = OK ){
	return err;
}

// if we reach here every thing went ok
return OK;

} 
int r1( void ){
int err;

err = printWelcome( void );

if ( err != OK ){
	return err;
}
int flag = 0;
while( flag != DONE ){
	
	// display prompt
	// accept command
	// analize command
	// execute command

}


}