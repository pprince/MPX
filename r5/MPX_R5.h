	# define COM1BASE 0x3F8  // also LSB
	# define INTID 0x0C // recheck this
	# define INTEN COM1BASE+1  // also MSB
	# define INTID_REG COM1BASE+2 
	# define LC COM1BASE+3 
	# define MC COM1BASE+4 
	# define LS COM1BASE+5 
	# define MS COM1BASE+6 
	
	# define PIC_MASK 0x21
	# define EOI 0x20
	# define OPEN 0
	# define CLOSED 1
	# define AVAILABLE 0
	# define IN_USE 1
	# define READ 0
	# define WRITE 1
	# define IDLE 2
	
	
	typedef struct deviceControlBlock{
		char ringBugger[1000];
		int ringSize,get,put;
		
		int ocFlag,status,currentOp,requested,transferred;
		char* Ibuff,Obuff;
		
	}DCB;
		
	//prototypes
	int com_open(int *, int ); 
	int com_close(void); 
	int com_read(char *, int *); 
	int com_write(char *, int *); 
	
	void setvect(int int_ID,void interrupt(*handler)(void));
	void interrupt(*getvect(int int_ID))(void);
	void disable(void);
	void enable(void);
	void outportb (int port_id, unsigned char value);
	unsigned char inportb (int port_id);
	

