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
	
	int com_close (void){}
	
	void interrupt level1(){
		//check if open, check to find out what lvl 2 interrupt then call
		int temp;
		if(dcb->ocFlag==CLOSED){
			outportb(EOI,EOI);
			return;
		}
		
		else{
			temp=inportb(INTID_REG&&0x07); // 0011 1111 1001 and 0000 0111
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
		//check for write status, check to see if required chars sent if not send a char, if done set to idle and wrap up
		int mask;
		
		if(dcb->status!=WRITE){
			return;
		}
		
		if((dcb->request)<(*dcb->count)){
			outportb(COM1BASE, *dcb->Obuff);
			dcb->Obuff++;
			dcb->request++;
		}
		
		else{
			dcb->status=IDLE;
			*dcb->ocFlag=OPEN;
	
			mask = inportb(INTEN);
			mask = mask&~0x02;  //0000 1101 and 0000 0010
			outportb(INTEN, mask);
		}
			
}
	
	void level2Read(){
		//check if reading then read the incoming char, if not put the char into the buffer
		char tempChar=inportb(COM1BASE);
		if(dcb->status==READ){
			dcb->Ibuff=tempChar;
			dcb->Ibuff++;
			dcb->count++;
			
			if(*(dcb->Ibuff) == '\r'){
				*(dcb->Ibuff) = '\0';
			}
			
			if(dcb->request==dcb->trans){
				*dcb->ocFlagPtr=OPEN;
				dcb->status=IDLE;	
			}
		}
	
		else if((dcb->status!=READ)&&((dcb->count)<(dcb->ringSize))){
			dcb->ringBuffer[dcb->put]=tempChar;
			dcb->put++;
			dcb->trans++;
		}
	
	}
	
	int com_read (char *buf_p,int *count_p){
		// check if null, open, idle, then set variables, disable interupts then read in until full or done, enable interupts, finish up
		if((buf_p==NULL)&&(&count_p==NULL)&&(dcb->status==IDLE)&&(dcb->ocFlag==CLOSED)){
			return;
		}
		
		dcb->Ibuff=buf_p;
		dcb->count=count_p;
		dcb->trans=0;
		dcb->ocFlag=0;
		
		disable();
		dcb->status=READ;
		while(){ // add conditions
			dcb->Ibuff++;
			dcb->trans=0;
			dcb->get++;
			dcb->rSize--;	
		}
		enable();
		
 
 
		*dcb.Ibuff= '\0';

		dcb->status=IDLE;
		*dcb->ocFlag=SET;
	 	*dcb->count=dcb->get;
		
	}
	
	int com_write (char *buf_p,int *count_p){
		//check if null, open, idle, then set variables, write to the buffer, call outportb, finish up
		int mask;
		
		if((buf_p==NULL)&&(&count_p==NULL)&&(dcb->status==IDLE)&&(dcb->ocFlag==CLOSED)){
			return;
		}
		
		dcb->status=WRITE;
		*dcb->ocFlag=CLOSED;
		dcb->trans=0;
		dcb->Ibuff=buf_p
		dcb->count=count_p;
		
		outportb(COM1BASE,*Ibuff);
		
		dcb->trans++;
		dcb->Ibuff++;
  		mask = inportb(INTEN); 
	 	mask = mask | 0x02;  // 0000 1101 or 0000 0010
	 	
	 	outportb(INTEN,mask); 
	}

