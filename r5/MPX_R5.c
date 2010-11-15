# include "MPX_R5.h"
# include <stdlib.h> 
# include <dos.h> 

	DCB dcb;

	int com_open (int *eflag_p, int baud_rate){
		//check for closed,null,invalid, set variables , send values to ports, disable send 0x31 enable
 		long brd;
    	int mask;
		
		if((dcb->ocFlag==OPEN)&&(eflag_p==NULL)&&(baud_rate<=0)){
			return;
		}
		
		dcb->status=IDLE;
		dcb->ocFlag=OPEN;
		dcb->ocFlagPtr=eflag_p;
		dcb->count=0;
		dcb->Ibuff=0;
		dcb->Obuff=0;
		
	  	oldfunc = getvect(INTID); 
	    setvect(INDID, &level1);     
	    brd=115200/(long)baud_rate; 
	    outportb(LC, 0x80); // set to 1000 1000 
	    outportb(COM1BASE, brd & 0xFF); // xxxx xxxx and 1111 1111 = xxxx xxxx
	    //check this why not just make it brd?
	    outportb(INTEN,(brd>>8) & 0xFF); // shift xxxx xxx to the right 8 times then and 1111 1111
	    //check this why not just shift it to the right eight and wouldnt that put it back to its original?
	    outportb(LC, 0x03); // set to 0000 0011
 

		disable(); 
		mask = inportb(PIC_MASK); 
		mask = mask & ~0x10; //0010 0001 and 0001 0000 = 0000 0000 
		// check this it doesnt make sense
		outportb(PIC_MASK, mask);
		enable(); 
		
		outportb(INTEN, 0x01); // set to 0000 0001
  		outportb(MC, 0x08); // set to 0000 1000
  		
		return 0;	
	}
	
	int com_close (void){
		// check if open, disable send 0x31 enable, set stuff to 0x00
 		int mask; 
		
		if(dcb->ocFlag!=OPEN){
	        return;
		}
	         
	    dcb->ocFlag=CLOSED; 
	    
	    disable(); 
	    mask = inportb(PIC_MASK); 
	    mask = mask | 0x10; //0010 0001 or 0001 0000 = 0011 0001 = 0x31
	    outportb(PIC_MASK, mask); 
	    enable(); 
	    
	    outportb(INTEN,0x00);      
	    outportb(MS,0x00); 
	    setvect(INTID,oldfunc); 
    
    	return 0;
	}
	
	void interrupt level1(){
		//check if open, check to find out what lvl 2 interrupt then call
		int temp;
		if(dcb->ocFlag==CLOSED){
			outportb(EOI,EOI); // send 0x20 to port 0x20
			return;
		}
		
		else{
			temp=inportb(INTID_REG && 0x07); // xxxx xxxx and 0000 0111 = 0000 0100 or 0000 0010
			if(temp==2){ // 0000 0010
				level2Write();
			}
			if(temp==4){ // 0000 0100
				level2Read();
			}
			outportb(EOI,EOI);
		}
	}
	
	void level2Write(){
		//check for write status, check to see if required chars sent, if done set to idle and wrap up
		int mask;
		
		if(dcb->status!=WRITE){
			return;
		}
		
		if((dcb->trans)<(*dcb->Ocount)){
			outportb(COM1BASE, *dcb->Obuff);
			dcb->Obuff++;
			dcb->trans++;
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
			*dcb->Ibuff=tempChar;
			dcb->request++;
			dcb->Ibuff++;
			
			if(*(dcb->Icount)<=(dcb->request){ // checks for done
				*(dcb->Ibuff-1) = '\0';
			}
						
			if(tempChar == '\r'){  //check for return key
				*(dcb->Ibuff) = '\0';
			}
			
			if((*(dcb->Icount)<=(dcb->request)||tempChar == '\r')){
				*dcb->Icount=dcb->request;
				*dcb->ocFlagPtr=OPEN;
				dcb->status=IDLE;	
			}
		}
	
		else if((dcb->count)!=512){
			dcb->ringBuffer[dcb->put]=tempChar;
			dcb->put++;
			dcb->count++;
		}
	
	}
	
	int com_read (char *buf_p,int *count_p){
		// check if null, open, idle, then set variables, disable interupts then read in until full or done, enable interupts, finish up
		if((buf_p==NULL)&&(&count_p==NULL)&&(dcb->status==IDLE)&&(dcb->ocFlag==CLOSED)){
			return;
		}
		
		*dcb->ocFlagPtr=OPEN;
		dcb->trans=0;
		dcb->Ibuff=buf_p;
		dcb->count=count_p;
		
		disable();
		dcb->status=READ;
		while(((dcb->Ibuff-1)!='\r')&&(dcb->count!=0)&&((*dcb->Icount)<=(dcb->request)){ // check for return key, full buffer, not overwrite unread chars
			dcb->trans=0;
			dcb->Ibuff++;
			dcb->put++;
			dcb->count--;	
		}
		enable();
		
		if(!((dcb->request)< *(dcb->Icount))){ // check to see if not full
			
			if(dcb->Ibuff-1== '\r'){
				*dcb->Ibuff-1= '\0';
			}
			
			else{
				*dcb->Ibuff= '\0';	
			}
	
			*dcb->ocFlagPtr=OPEN;
		 	*dcb->Icount=dcb->trans;
		 	dcb->status=IDLE;
		} 	
		
	 	return 0;		
	}
	
	int com_write (char *buf_p,int *count_p){
		//check if null, open, idle, then set variables, write to the buffer, call outportb, finish up
		int mask;
		
		if((buf_p==NULL)&&(&count_p==NULL)&&(dcb->status==IDLE)&&(dcb->ocFlag==CLOSED)){
			return;
		}
		
		dcb->status=WRITE;
		*dcb->ocFlagPtr=CLOSED;
		dcb->request=0;
		dcb->Obuff=buf_p
		dcb->Ocount=count_p;
		
		outportb(COM1BASE,*dcb->Obuff);
		
		dcb->request++;
		dcb->Obuff++;
  		mask = inportb(INTEN); 
	 	mask = mask | 0x02;  // 0000 1101 or 0000 0010 = 0000 1111 = 0x0F
	 	
	 	outportb(INTEN,mask); 
	 	
	 	return 0;
	}

