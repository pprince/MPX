#include "mpx_supt.h"
#include "mpx_r1.h" // Bring in R1 code

void main(){
	int error;
	sys_init( MODULE_R1 ); //System initilization 
	error = r1();
	printf("Error: %d\n", error); 
	sys_exit(); // MPX System Exit functions
}
