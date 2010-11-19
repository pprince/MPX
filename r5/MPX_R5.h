	//COM1 Addresses 
#define INT_ID 0X0C // interupt ID for windows interupt table
#define BASE 0x3F8  //com1 base address
#define INT_EN BASE+1  // interupt enable
#define BRD_LSB BASE // LSB Baud Rate Devisor
#define BRD_MSB BASE+1 // MSB Baud Rate Devisor
#define INT_ID_REG BASE+2
#define LC BASE+3 // Line control register
#define MC BASE+4 // Modem Control Register
#define LS BASE+5 // Line control status
#define MS BASE+6 // Modem Control status
#define PIC_MASK 0X21 // Programmable Interupt Controler Mask
#define PIC_CMD 0x20  // Programmable Interupt Controler Command
#define EOI 0x20

//Used in interrupt1
#define WHATINTERRUPTBIT 0x07

//ring buffer size
#define size 256

//Flag states
#define OPEN 1 //device open
#define CLOSED 0 //device closed
#define SET 1 //event flag set
#define FLAG_CLEAR 0 //event flag cleared
#define IDLE   0
#define READ   1
#define WRITE  2

//Error values returned
#define NO_ERROR 0 //no error
#define INV_FLAG -101 //invalid even flad pointer
#define INV_BAUD -102 //invalid baud rate divisor
#define PORT_ALREADY_OPEN -103 //port already open
#define SERIAL_PORT_NOT_OPEN -201 //serial port not open
#define READ_PORT_NOT_OPEN -301 //port not open
#define READ_INV_BUFF_ADD -302 //invalid buffer address
#define READ_INV_COUNT -303 //invalid count address or count value
#define READ_DEV_BUSY -304 //device is busy
#define WRITE_PORT_NOT_OPEN -401
#define WRITE_INV_BUFF_ADD -402
#define WRITE_INV_COUNT -403
#define WRITE_DEV_BUSY -404
   
//global variables 
int callInt=0; 
int num; 
  
typedef struct device{ 
	int flag; //indicate if device is open or closed. 
	int *flag_ptr; //pointer to event flag 
	int  status; //status code: IDLE, READ, WRITE 
	char *inbuff; //pointer to requester's buffer; read data placed here 
	int *incount; //max number of chars can be placed in requester's buffer 
	int indone; //number of chars that have been placed in requester's buffer 
	char *outbuff; 
	int *outcount; 
	int outdone; 
	char ringbuf[size]; //ring buffer 
	int ringbufin; //where write next char will be placed 
	int ringbufout; //where remove next char from buffer 
	int ringbufcount; //number of stored but not read chars from buffer 
}DCB; 
   
//typedef struct device DCB; 
DCB dcbPtr;
   
//prototypes 
int com_open(int *, int ); 
int com_close(void); 
int com_read(char *, int *); 
int com_write(char *, int *); 
void interrupt level1();
void level2Write();
void level2Read();
void interrupt (*oldfunc) (void);  
	
