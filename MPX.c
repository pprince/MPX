#include <MPX_SUPT.c>
#include <MPX_SUPT.c>
#include <MPX_R1.c> // Bring in R1 code

void main(){
 int error;
 int sys_int( MODULE_R1 ); //System initilization 
 error =  r1( void );
 printf(error); 
 void sys_exit( void ); // MPX System Exit functions
}