/***********************************************************************
	MPX: The MultiProgramming eXecutive
	Project to Accompany
	A Practical Approach to Operating Systems
	Malcolm G. Lane & James D. Mooney
	Copyright 1993, P.W.S. Kent Publishing Co., Boston, MA.

	File Name:	MPX_R5.c

	Author:	Nathaniel Clay and Nicholas Yanak
	Version: 1.1
	Date:  12/9/2010

	Purpose: This is a com port driver for interfacing with the com port

		
	Environment: Windows XP 32 bit

************************************************************************/
# include "mpx_supt.h"
# include <stdlib.h> 
# include <dos.h> 
# include "MPX_R5.h"
	/* This function opens the com port for read or write*/
	int com_open (int *eflag, int baudrate){
		long brd;
		int mask;

		if(eflag == NULL)
			return INV_FLAG; // invalid flag
		if(baudrate <= 0)
			return INV_BAUD;  // invalid baud
		if(dcbPtr.flag==OPEN) // Make sure that the device is not open.
			return PORT_ALREADY_OPEN;
		
		// set initial values
		dcbPtr.flag = OPEN;
		dcbPtr.flag_ptr = eflag;
		dcbPtr.status = IDLE; 
		dcbPtr.ringbufin = 0; 
		dcbPtr.ringbufout = 0; 
		dcbPtr.ringbufcount = 0;
          
		oldfunc = getvect(INT_ID); //get the vector of the Windows comport interupt handler  
		setvect(INT_ID, &level1); //level1 is interrupt handler      
		brd = 115200 / (long) baudrate; //calculate baud rate divisor
         
		
		outportb(LC, 0x80); //store 0x80 in line control register 
		outportb(BRD_LSB, brd & 0xFF); //is Baud rate devisor LSB       
		outportb(BRD_MSB,(brd>>8) & 0xFF); //is Baud rate devisor MSB   
		outportb(LC, 0x03); //store 0x03 in line control register 
        
		disable(); // disable interupts 
		mask = inportb(PIC_MASK); 
		mask = mask & 0xEF; 
		outportb(PIC_MASK, mask); 
		enable(); // enable interupts
         
		//enable level for COM1 in PIC Mask register 
		//Store 0x08 in modem control register 
		outportb( MC, 0x08);//enables serial interrupts 
		//store 0x01 in interrupt enable register 
		outportb( INT_EN, 0x01);//enables input ready interrupts 
         
    return 0; // return zero if no error.
	}
	
	/* This function closes the com port*/
	int com_close (void){
		int mask; 
	
		if(dcbPtr.flag != OPEN) //check that port is open
			return SERIAL_PORT_NOT_OPEN; 
         
		dcbPtr.flag=CLOSED; 
		disable(); //start enable 
		mask = inportb(PIC_MASK); 
		mask = mask | 0x10; 
		outportb(PIC_MASK, mask); 
		enable(); //end enable 
         
		outportb( MS,0x00); // clears Modem Control Status
		outportb( INT_EN,0x00); // clears int_en
		setvect( INT_ID, oldfunc); //restore Microsoft interupt 
         
		return 0; 
 	
	}
	
	/* This function is the interupt handler that decides whether to perform a read or write*/
	void interrupt level1(){
		if(dcbPtr.flag != OPEN){ 
			outportb(PIC_CMD, EOI);//clear interupt PIC command register 
			return; 
		} else{ 
			num = ((inportb(INT_ID_REG) & WHATINTERRUPTBIT)); // xxxx xxxx and 0000 0111 = 0000 0xxx
			if (num == 2) // 0000 0010 : write interrupt 
				level2Write(); 
			if (num == 4) // 0000 0100 : read interrupt 
				level2Read();     
			outportb(PIC_CMD, EOI); //clear interupt PIC command register 
		} 
    return; 

	}
	
	/* This function performs a write operation*/
	void level2Write(){

		int mask; 
         
		if(dcbPtr. status != WRITE) 
			return; //Ignore the interrupt and return

		if(dcbPtr.outdone < *dcbPtr.outcount){ // checks too see if all the data has been written if not go until done
			outportb(BASE, *dcbPtr.outbuff);
			dcbPtr.outbuff++; 
			dcbPtr.outdone++;
			return;
		}else{
			// set the com port to idle and set the flag
			dcbPtr.status = IDLE;
			*dcbPtr.flag_ptr = SET;

			mask = inportb(INT_EN);
			mask = mask&~0x02; 
			outportb(INT_EN, mask); ///< and INT_EN with 1111 1101 and send it back
			return;
		}
	}
	/* This is a reaad interupt*/
	void level2Read(){
		char new;
	
	
		//If the current status is not reading, store the character in the ring buffer. 
		if(dcbPtr.status != READ){
			if(dcbPtr.ringbufcount != size){
				dcbPtr.ringbuf[dcbPtr.ringbufin]= new;
				dcbPtr.ringbufin = (dcbPtr.ringbufin+1)%size;
				dcbPtr.ringbufcount++;
			}
		}else{ //status is reading
			*dcbPtr.inbuff = new;
			dcbPtr.inbuff++;
			dcbPtr.indone++;
				//If the count is not completed and the character is not CR, return. Do not signal completion. 
			if(new== '\r' || (dcbPtr.indone ) >= *(dcbPtr.incount)){
				if(*(dcbPtr.inbuff-1) == '\r'){
					*(dcbPtr.inbuff-1) = '\0';
				}else{
					*dcbPtr.inbuff = '\0';
				}
				*dcbPtr.incount = dcbPtr.indone;
				dcbPtr.status =IDLE;
				*dcbPtr.flag_ptr = SET;
		}//end if
	}//end else

	}
	/* this functions reads data from the com port*/
	int com_read (char *buf_p,int *count_p){
		//Validate the supplied parameters. 
		if(dcbPtr.flag != OPEN) //check if device is open
			return READ_PORT_NOT_OPEN;
		if(dcbPtr.status != IDLE) //check if device is idle
			return READ_DEV_BUSY;
		if( buf_p == NULL) //check if buffer is empty 
			return READ_INV_BUFF_ADD; 
		if( &count_p == NULL) //check if count pointer is null 
			return READ_INV_COUNT;  
        // Initialize the input buffer variables (not the ring buffer!) and set the status to reading. 
		dcbPtr.inbuff = buf_p; 
		dcbPtr.incount = count_p; 
		dcbPtr.indone = 0; 

		*(dcbPtr.flag_ptr) = FLAG_CLEAR; //clear event flag
		
		disable(); //disable interrupts 
		dcbPtr.status=READ; //we are now reading

		/* Copy characters from the ring buffer to the requestor's buffer, 
		until the ring buffer is emptied, the requested count has been 
		reached, or a CR (ENTER) code has been found. The copied 
		characters should, of course, be removed from the ring buffer.
		Either input interrupts or all interrupts should be disabled 
		during the copying. */ 

		while((dcbPtr.ringbufcount >0) && (dcbPtr.inbuff-1 !='\r' && (dcbPtr.indone >= *(dcbPtr.incount)))){
				*((dcbPtr.inbuff)) = dcbPtr.ringbuf[dcbPtr.ringbufout]; 
				dcbPtr.indone++; 
				dcbPtr.inbuff++;
				dcbPtr.ringbufout = (dcbPtr.ringbufout+1)%size;
				dcbPtr.ringbufcount--;
				
			} //end while 
          
			enable(); //enable interrupts 
	
		//the requestor buffer is not yet full     
		if(dcbPtr.indone < *(dcbPtr.incount)) 
			return 0;  
		if(*(dcbPtr.inbuff-1) == '\r') ///< if last char is CR then set it to null char
			*(dcbPtr.inbuff-1) = '\0';
		else
			*dcbPtr.inbuff = '\0';
		
		//Reset the DCB status to idle, set the event flag, and
		//return the actual count to the requestor's variable. 
		dcbPtr.status = IDLE; //status back to IDLE 
		*dcbPtr.flag_ptr = SET; //the event is over 
		*dcbPtr.incount = dcbPtr.indone; 
	 return 0;
	}
	/* this functions writes a char to the com port*/
	int com_write (char *buf_p,int *count_p){
		int mask; 
		//Ensure that the input parameters are valid. 
		if(dcbPtr.flag != OPEN) //check if device is open 
			return WRITE_PORT_NOT_OPEN; 
		if(dcbPtr.status != IDLE) //check if device is idle 
			return WRITE_DEV_BUSY; 
		if(buf_p == NULL) //check if buffer is empty 
			return WRITE_INV_BUFF_ADD; 
		if(count_p == NULL) //check pointer is null 
			return WRITE_INV_COUNT; 
		
		//Install the buffer pointer and counters in the DCB, and set the current status to writing. 
		dcbPtr.outbuff = buf_p; 
		dcbPtr.outcount = count_p; 
		dcbPtr.outdone = 0; 
		dcbPtr.status = WRITE;
		//Clear the caller's event flag. 
		*dcbPtr.flag_ptr = FLAG_CLEAR; 
		
		//Get the first character from the requestor's buffer and store it in the output register. 
		outportb(BASE, *dcbPtr.outbuff); 
		dcbPtr.outbuff++; 
		dcbPtr.outdone++; 
         
		 
		 //Enable write interrupts by setting bit 1 of the Interrupt Enable register. 
		 //This must be done by setting the register to the logical or of its previous 
		 //contents and 0x02. 

		mask = inportb(INT_EN); 
		mask = mask | 0x02; 
		outportb(INT_EN,mask); 
    
	return 0; 
	}

