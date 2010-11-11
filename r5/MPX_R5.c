# include "MPX_R5.h"
# include <stdlib.h> 
# include <dos.h> 

	DCB dcb;

	int com_open (int *eflag_p, int baud_rate){
		//check for closed,null,invalid, set variables , send values to ports, disable send 0x31 enable
 		long brd;
    	int mask;
		
		if((dcb->ocFlag==OPEN)&&(eflag_p!=NULL)&&(baud_rate<=0)){
			return;
		}
		
		dcb->status=IDLE;
		dcb->ocFlag=OPEN;
		dcb->ocFlagPtr=eflag_p;
		dcb->count=0;
		dcb->Ibuff=0;
		dcb->Obuff=0;
		
	  	oldfunc = getvect(INTID); 
	    setvect(INDID, & level1);     
	    brd=115200/(long)baud_rate; 
	    outportb(LC, 0x80); // set to 1000 1000 
	    outportb(COM1BASE, brd & 0xFF); // xxxx xxxx and 1111 1111 = xxxx xxxx
	    //check this why not just make it brd?
	    outportb(INTEN,(brd>>8) & 0xFF); // shift xxxx xxx to the right 8 times then and 1111 1111
	    //check this why not just shift it to the right eight and wouldnt that put it back to its original?
	    outportb(LC, 0x03); // set to 0000 0011
 

		disable(); 
		mask = inportb(PIC_MASK); 
		mask = mask | 0x10; //0010 0001 or 0001 0000 = 0011 0001 = 0x31
		outportb(PIC_MASK, mask);
		enable(); 
		
		outportb(INTEN, 0x01); // set to 0000 0001
  		outportb(MC, 0x08); // set to 0000 1000
  		
	
	}
	
	int com_close (void){
		// check if open, disable send 0x31 enable, set stuff to 0x00
 		int mask; 
		
		if(dcb->ocFlag!=OPEN) 
	        return; 
	         
	    dcb->ocFlag=CLOSED; 
	    
	    disable(); 
	    mask = inportb(PIC_MASK); 
	    mask = mask | 0x10; //0010 0001 or 0001 0000 = 0011 0001 = 0x31
	    outportb(PIC_MASK, mask); 
	    enable(); 
	    
	    outportb(INTEN,0x00);      
	    outportb(MS,0x00); 
	    setvect(INTID,oldfunc); 
    
	}
	
	void interrupt level1(){
		//check if open, check to find out what lvl 2 interrupt then call
		int temp;
		if(dcb->ocFlag==CLOSED){
			outportb(EOI,EOI);
			return;
		}
		
		else{
			temp=inportb(INTID_REG&&0x07); // 0011 1111 1001 and 0000 0111 = 0000 0001 = 0x01
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
			mask = mask & 0x02;  //0000 1101 and 0000 0010 = 0000 0000 = 0x00 
			//why not name this mask = 0 check this?
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
	 	mask = mask | 0x02;  // 0000 1101 or 0000 0010 = 0000 1111 = 0x0F
	 	
	 	outportb(INTEN,mask); 
	}

