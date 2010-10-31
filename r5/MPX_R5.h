	# define COM1 0x3F8 
	#define PIC_MASK 0x21
	
	typedef struct deviceControlBlock{
		status,current,eventFlag,Ubuffer,Ibuffer,
		requested,transferred,temp,
	}DCB;
		
	//prototypes
	int com_open (int *eflag_p, int baud_rate);
	int com_close (void);
	int com_read (char *buf_p,int *count_p);
	int com_write (char *buf_p,int *count_p);
	void setvect(int int_ID,void interrupt(*handler)(void));
	void interrupt(*getvect(int int_ID))(void);
	void disable(void);
	void enable(void);
	void outportb (int port_id, unsigned char value);
	unsigned char inportb (int port_id);
	

