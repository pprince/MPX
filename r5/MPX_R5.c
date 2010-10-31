# include "MPX_R5.h"
#include <stdlib.h> 
#include <dos.h> 


	...
	int mask;
	...
	disable();
	mask = inportb(PIC_MASK);
	mask = mask & ~0x80;
	outportb(PIC_MASK, mask);
	enable();

	int baud rate=115200/1200;
	baud_rate_div = 115200 / (long) baud_rate;






	char ringBugger[1000],size=0,get=0,put=0;

	int push(char 'c'){
		if(size >= sizeof(ringBuffer)){
			return -1;	
		}
		
		ringBuffer[put]='c';
		put++;
		
		if(put >= sizeof(ringBuffer)){
		put = 0; // sets placement back at the beggining
		size++;
		return 0;
		}
	}
	
	int pop(void){
		char c;
		if (size==0){
			return -1;
		}
	
		size--;
		c = ringBuffer[get];
		get++;
		
		if (get >= sizeof(ringBuffer){
			get=0;
			return c;
		}
	}
	
