#include "mpx_cmd.h"
#include "mpx_util.h"
#include "mpx_supt.h"

/**
* Entry Point of MPX. 
* This is the entry point of MPX, it calls the mpx_command_loop. The Command Loop function
* starts the display of the Welcome Message and the inital home screen of MPX.
*/
void main(){
	int err;
	sys_init( MODULE_R1 + MODULE_R2 ); //System initilization 
	err = mpx_command_loop();
	errorDecode(err);
}
