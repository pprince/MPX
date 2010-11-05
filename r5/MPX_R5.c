# include "MPX_R5.h"
# include <stdlib.h> 
# include <dos.h> 

	DCB dcb;

	int com_open (int *eflag_p, int baud_rate){
		if(((eflag_p||baud_rate)==NULL)||(baud_rate<=0)||(dcb->ocFlag==OPEN)){
			return;
		}
		
	baud_rate_div = 115200 / (long) baud_rate

	...
	int mask;
	...
	disable();
	mask = inportb(PIC_MASK);
	mask = mask & ~0x80;
	outportb(PIC_MASK, mask);
	enable();
	
	}
	
	void interrupt level1(){
		//check if open, check to find out what lvl 2 interrupt then call
		int temp;
		if(dcb->ocFlag==CLOSED){
			outportb(EOI,EOI);
		}
		
		else{
			temp=inportb(INTID_REG&&0x07); //bit by bit and xxxx xxxx && 0000 0111
			if(temp==2){
				level2Write();
			}
			if(temp==4){
				level2Read();
			}
			outportb(EOI,EOI);
		}
	}
	
	void level2Write(){
	}
	
	void level2Read(){
	}
	
	
	int com_close (void);
	int com_read (char *buf_p,int *count_p);
	int com_write (char *buf_p,int *count_p);


	int pushRing(char 'c'){
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
	
	int popRing(void){
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
	
