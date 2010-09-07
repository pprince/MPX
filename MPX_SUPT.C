/***********************************************************************
	MPX: The MultiProgramming eXecutive
	Project to Accompany
	A Practical Approach to Operating Systems
	Malcolm G. Lane & James D. Mooney
	Copyright 1993, P.W.S. Kent Publishing Co., Boston, MA.
	
	File Name: mpx_supt.c

	Authors: J.J. Patnesky, A. Ghosal, M. Lane, J. Mooney
	Version: 2.1b
	Date: 09/18/95

	Purpose: Support routines for MPX-PC
	
	Modules: all

	Environment:      IBM-PC (XT, AT or PS/2)
			MS-DOS 3.3 or later
			TURBO-C 3.0 or TURBO-C++ 1.0 or later

	Compile using the LARGE MODEL and SS NOT equal to DS.
	All addresses and pointers are "far", and
	CS, DS and SS are independent.



	Procedures in this file:

		sys_init
		sys_exit
		sys_set_vec
		sys_req
		sys_alloc_mem
		sys_free_mem
		sys_get_date
		sys_set_date
		sys_open_dir
		sys_get_entry
		sys_close_dir
	       sys_check_program
	       sys_load_program




************************************************************************

Change Log:

	07/16/88  akg	added comments
	02/25/92  jdm	full revision, UNIX version
	04/14/92  jdm	Macintosh version
	05/15/92  jdm	cleanup and documentation
	07/14/92  jdm	update for revised specs
        08/13/92  jdm 	revised IBM version; added private heap
        10/14/92  jdm 	added check and load procedures
        10/18/92  jdm 	revised sys_alloc_mem for alignment
	11/17/92  jdm	true heap mgt, aligned, using calloc & free
	11/19/92  jdm	enable device handling for final module
	12/31/92  jdm	revised sys_req parameters
	01/02/93  jdm	return count value after I/O
	01/03/93  jdm	revised error names and codes
	01/27/93  jdm	corrected count for line input in sys_req
			fixed open_dir to handle null lines
	02/24/93  jdm	changed sys_set_vec param to interrupt
	03/08/93  jdm	corrections to sys_check & sys_load
	03/20/93  jdm	stack switch in free, check, and load
	04/22/93  jdm	corrected index conflict in sys_free_mem
			added safety escape in alloc table search
	07/28/93  jdm	conversion to large model
	09/01/93  jdm	fixed allocation bug;  made date private
	09/14/93  jdm	cleaned up memory allocation
        09/16/93  jdm	adjusted program load for large model
        09/30/93  jdm	modified for correct list of root directory
	11/01/94  jdm	changed load file read to binary
	09/18/95  jdm	fixed get_entry to default to current, not root

************************************************************************/

#include "mpx_supt.h"
#include <stdlib.h>
#include <dos.h>
#include <dir.h>
#include <errno.h>
#include <conio.h>
#include <alloc.h>

#define VEC_ADDR (0x60L*4)

#define MAX_ALLOC 200

#define MAX_XPOS 79
#define MAX_YPOS 24

#define MAX_PATH_SIZE 50

/*
	System-dependent type definitions
*/

typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long longword;
typedef byte *address;

typedef struct params {
	int      op_code;
	int      device_id;
	char     *buf_p;
	int      *count_p;
	} params;

/*
	Global variables accessed only by support routines
*/

	static longword vec_save;  /* saved interrupt vector */
	static int mod_code;                /* module code */                                           
	static struct date sys_date;        /* MPX system date */

	/* data structures for directory access */
        static char current_path[MAX_PATH_SIZE+1];
        static int num_entries;
        static struct ffblk file_block;

        /* handler presence flags */
	static flag sysc_hand;
	static flag trm_hand;
	static flag prt_hand;
	static flag com_hand;

        /* memory allocation table */
	static struct {
		void* original;
		void* aligned;
	} alloc_table[MAX_ALLOC];
	static int alloc_ix;                /* current index */
	static int num_alloc;               /* no. of allocated blocks */



/*
	Procedure: sys_init

	Purpose: initialize mpx

	Parameters: see prototype  

	Return value: error code, or OK if no error

	Calls: getdate    

	Globals: mod_code
		vec_save
		sys_date
		alloc_table, alloc_ix, num_alloc
		sysc_hand, term_hand, prt_hand, com_hand

	Errors: none
*/
int sys_init (    int      modules  /* module code */
	     )

{
	mod_code = modules;
        vec_save = 0L;


        /* no handlers detected yet */
	sysc_hand = FALSE;
	trm_hand = FALSE;
	prt_hand = FALSE;
	com_hand = FALSE;

	/* initialize allocation table */
	for (alloc_ix=0; alloc_ix < MAX_ALLOC; alloc_ix++) {
		alloc_table[alloc_ix].original = NULL;
		alloc_table[alloc_ix].aligned = NULL;
	}
	alloc_ix = 0;
	num_alloc = 0;

	/* if we have reached Module R3, enable system call handling */
	if (modules >= MODULE_R3) sysc_hand = TRUE;

	/* if we have reached the final module, enable device handlers */
	if (modules >= MODULE_F) {
		trm_hand = TRUE;
		prt_hand = TRUE;
		com_hand = TRUE;
	}

	/* get system date */
        getdate(&sys_date);

	return (OK);
}

/*
	Procedure: sys_exit

	Purpose: terminate mpx

	Parameters: none  

	Return value: none

	Calls: none       

	Globals: vec_save
*/
void sys_exit(void)

{
	longword *vec_p;

	/* if trap vector changed, restore it */
	if (vec_save != 0L) {
		vec_p = (longword*) VEC_ADDR;
		*vec_p = vec_save;
	}

	/* return to host with null error code */
	exit(0);
}


/*
	Procedure: sys_set_vec

	Purpose: link trap vector to MPX system call handler

	Parameters: handler: addr of handler         

	Return value: error code; OK if no error

	Calls: none       

	Globals: vec_save

	Errors: none
*/
int sys_set_vec   ( void interrupt (*handler)() /* system call handler */
		)

{
	longword *vec_p;
	
		vec_p = (longword*) VEC_ADDR;
		vec_save = *vec_p;
		*vec_p = (longword) handler;

	return (0);
}


/*
	Procedure: sys_req

	Purpose: Request a system service

	Inputs:

		op_code           operation code
		device_id         device identifier
		buf_p             addresso f data buffer
		count_p           address of size of buffer

	Returns: Result or error code, depending on service
	
	Services supported:	IDLE     null operation
				READ     input from a device
				WRITE    output to a device
				CLEAR    clear terminal screen
				GOTOXY   absolute cursor position
				EXIT     terminate the caller

	Calls:   fgets
		strlen
		
	Globals: none
	
	Errors:  ERR_SUP_INVDEV    invalid device
		ERR_SUP_INVOPC    invalid operation code
		ERR_SUP_INVPOS    invalid character position
		ERR_SUP_RDFAIL    read failed
		ERR_SUP_WRFAIL    write failed

	Description:

	For Modules R1 through R4,
	this procedure is used only for terminal I/O.
	Later modules support both reading and
	writing for several devices.

	For Modules R1 through R4,
	sys_req uses the ANSI C function "fgets."
	Later modules may use alternate functions and device drivers.

*/

int sys_req (   int      op_code, /* operation code */
		int      device_id,        /* device id */
		char*    buf_p,            /* I/O buffer */
		int*     count_p           /* address of count */
		)

{
	int      rval;    /* result or error code */
	char     *rp;     /* return pointer for fgets */
	char     rc;      /* return char for fputc */
        params    *param_p; /* pointer to parameter record in stack */
	flag     docall;  /* true if system call interrupt wanted */
	int      ix;      /* temporary index */


	docall = FALSE;
	rval = OK;
	switch (op_code) {
	
	case IDLE:
		if (sysc_hand) docall = TRUE;
		break; 
		
	
	case READ:
		switch (device_id) {
		
		case TERMINAL:
			if (trm_hand) docall = TRUE;
			else {
				rp = fgets(buf_p,*count_p,stdin);
				if (rp==NULL) rval = ERR_SUP_RDFAIL;
				else rval = strlen(buf_p);
			}
			break;
			
		case COM_PORT:
			if (com_hand) docall = TRUE;
			else rval = ERR_SUP_INVDEV;
			break;
			
		default:
			rval = ERR_SUP_INVDEV;
			
		}
		break;

	case WRITE:
		switch (device_id) {
		
		case TERMINAL:
			if (trm_hand) docall = TRUE;
			else {
				rval = *count_p;
				for (ix=0; ix<*count_p; ix++) {
					rc = fputc(buf_p[ix],stdout);
					if (rc == EOF) {
						rval = ERR_SUP_WRFAIL;
						break;
					}
				}

			}
			break;
			
		case COM_PORT:
			if (com_hand) docall = TRUE;
			else rval = ERR_SUP_INVDEV;
			break;
			
		case PRINTER:
			if (prt_hand) docall = TRUE;
			else rval = ERR_SUP_INVDEV;
			break;
			
		default:
			rval = ERR_SUP_INVDEV;
			
		}
		break;


	case CLEAR:
		if (device_id==TERMINAL) {
			if (trm_hand) docall = TRUE;
			else {
			     clrscr();
				rval = 0;
			}
		}
		else rval = ERR_SUP_INVDEV;
		break;
		
	case GOTOXY:
		if (device_id==TERMINAL) {
			if (trm_hand) docall = TRUE;
			else {
				if (*count_p != 2) rval = ERR_SUP_WRFAIL;
				else if ((*buf_p<0)
					|| (*buf_p > MAX_XPOS))
					rval = ERR_SUP_WRFAIL;
				else if ((*(buf_p+1)<0)
					|| (*(buf_p+1) > MAX_YPOS))
					rval = ERR_SUP_WRFAIL;
				else {
					gotoxy(*buf_p + 1,
						*(buf_p+1) + 1);
					rval = 0;
				}
			}
		}
		else rval = ERR_SUP_INVDEV;
		break;
		
	/* EXIT - terminate the calling process */
	/* legal only when a system call handler is present */
	case EXIT:
		if (sysc_hand) docall = TRUE;
		else rval = ERR_SUP_INVOPC;
		break; 

	default:
		rval = ERR_SUP_INVOPC;
		
	}
	
	/* Invoke user's call handler if present.

	   If the call is an IO request, return will occur
	   only after the request is complete.
	*/
	if (docall && (rval==OK)) {

		/* invoke the call handler */
	_SP = _SP - sizeof(params);
		param_p = (params*) MK_FP(_SS,_SP);
	       param_p->op_code = op_code;
	       param_p->device_id = device_id;
	       param_p->buf_p = buf_p;
	       param_p->count_p = count_p;
	       geninterrupt(0x60);
	       rval = _AX;
	       _SP = _SP + sizeof(params);

		/* for I/O operations, return count value */
		switch(op_code) {

		case READ:
		case WRITE:
		case CLEAR:
		case GOTOXY:

			rval = *count_p;

		}


	}

	return(rval);

}


/*

	Procedure: sys_alloc_mem

	Purpose: Allocate a memory block

	Parameters:
	
		size_t size       No. of bytes to allocate

	Returns: void* pointer to allocated block;
		 null pointer in case of error
		 
	Calls: calloc
	
	Globals: alloc_table, alloc_ix, num_alloc


	For program loading (Module R-4), the blocks must be aligned.
	This is achieved by requesting one extra paragraph and
	returning the lowest aligned address as the base.  It is
	necessary to keep a table of allocated address, original and
	aligned, to support correct freeing.

	It would seem simpler to use the Turbo-C function allocmem,
	which always allocates aligned blocks.  However, allocmem
	appears to conflict with the internal allocation of fopen
	and other C library routines.


*/

void *sys_alloc_mem (      size_t   size     /* size in bytes to allocate */
		    )
{
	int ix_save;      /* temp copy of table index */
        void *addr;        /* addr returned by calloc (*void) */
	word offset;      /* offset of unaligned address */
	word seg;         /* segment addr of unaligned address */
	void *addr_alig; /* aligned address */
	int rem; /* temp for alignment computation */

	/* ensure that allocation table is not full */
	if (num_alloc >= MAX_ALLOC) {
		return(NULL);
	}

	/* find free entry in allocation table */
	ix_save = alloc_ix;
	while (alloc_table[alloc_ix].original != NULL) {
		alloc_ix++;
		if (alloc_ix >= MAX_ALLOC) alloc_ix = 0;

		/* safety check -- should never happen! */
		if (alloc_ix == ix_save) return(NULL);
	}

	/* call allocation routine */
	/* request 15 extra bytes to ensure alignment is possible */
	addr = calloc(size + 15,1);

	/* compute aligned base */
	offset = FP_OFF(addr);
        seg = FP_SEG(addr);
	rem = offset % 16;
	if (rem > 0) offset = offset + 16 - rem;
	addr_alig = MK_FP(seg,offset);

	/* insert entries in table */
	alloc_table[alloc_ix].original = addr;
	alloc_table[alloc_ix].aligned = addr_alig;


	/* increment count */
	num_alloc++;

	return(addr_alig);

}

/*

	Procedure: sys_free_mem

	Purpose: free a memory block

	Parameters:

		void *ptr         ptr to block to allocate

	Returns: integer error code; zero if OK
	
	Calls:   free

	Globals: alloc_table, num_alloc

	Errors:  ERR_SUP_INVMEM    invalid memory block


	This procedure receives an aligned address; it must
	find and free the corresponding non-aligned one.

*/

int sys_free_mem (         void     *ptr     /* pointer to block */
		 )
{
	void *free_addr;  /* true (unaligned) block address */
	int free_ix;               /* temp table index */

	/* ensure valid pointer */
	if (ptr==NULL) return(ERR_SUP_INVMEM);

	/* Look for the block in the allocation table */
	free_addr = NULL;
	for (free_ix=0; free_ix<MAX_ALLOC; free_ix++) {
		if (alloc_table[free_ix].aligned == ptr) {
			free_addr = alloc_table[free_ix].original;
			break;
		}
	}

	/* If the block wasn't found, report error */
	if (free_addr == NULL) return(ERR_SUP_INVMEM);


	/* free the block & clear the table entry */
	alloc_table[free_ix].original = NULL;
	alloc_table[free_ix].aligned = NULL;
        free(free_addr);

	/* decrement count */
	num_alloc--;

	return(OK);

}        

/*

	Procedure: sys_get_date

	Purpose: Returns the system date (month, day, year)

	Parameters:

		date_rec *date_p Pointer to structure of type date

	Returns: void
	
	Calls: none
	
	Globals: sys_date

        Errors: none

*/

void sys_get_date (  date_rec*      date_p   /* ptr to date record */
		)

{
	date_p->month = sys_date.da_mon;
	date_p->day = sys_date.da_day;
	date_p->year = sys_date.da_year;

}


/*

	Procedure: sys_set_date

	Purpose: Sets the system date (month, day, year)

	Parameters:

		date_rec *date_p ptr to structure of type date

	Returns: error code; OK if no error

	Calls:   none

        Globals: sys_date

	Errors: none
*/

int sys_set_date (  date_rec*       date_p   /* ptr to date record */
		 )

{
	sys_date.da_mon = date_p->month;
        sys_date.da_day = date_p->day;
        sys_date.da_year = date_p->year;

	return(OK);
}


/*
	Procedure: sys_open_dir

	Purpose: open a specified directory

	Parameters: char dir_name[]            directory name

	Returns: error code; zero if OK

	Calls:   findfirst
		strncpy  

	Globals: current_path, file_block, num_entries

	Errors:  ERR_SUP_DIROPN    directory open error
		ERR_SUP_INVDIR    invalid directory
*/


int sys_open_dir (  char  path_name[]        /* directory name */
		 )
{
        char temp_path[MAX_PATH_SIZE];
	int res;
        int len;

        /* ensure there is no directory currently open */
	if (current_path[0]!=NULCH) return(ERR_SUP_DIROPN);

        /* check pathname for validity */

        /* construct temporary search path name */
	temp_path[0] = NULCH;
	if (path_name != NULL) {
		strncpy(temp_path,path_name,MAX_PATH_SIZE-4);
	}

	/* build wildcard search string */
        len = strlen(temp_path);

	/* if path is null, use current directory */
        if (len==0) strcpy(temp_path,"*.*");

	/* handle paths ending in backslash, including root */
        else if (temp_path[len-1]=='\\') strcat(temp_path,"*.*");

	/* handle paths not ending in backslash */
        else strcat(temp_path,"\\*.*");

	/* validate the directory by searching for either an ordinary
	   file or a directory in it.  This should never fail; even
	   empty directories contain the directories "." and ".."
	*/
	
	res = findfirst(temp_path, &file_block, FA_DIREC);
	if (res!=0)  return(ERR_SUP_INVDIR);

        /* save pathname as current, clear entry count */
	current_path[0] = NULCH;
	if (path_name != NULL) {
		strncpy(current_path,path_name,MAX_PATH_SIZE-6);
	}
	num_entries = 0;

	return(OK);
}

/*
	Procedure: sys_get_entry

	Purpose: get the next directory entry;

	Parameters: char name_buf[]	name buffer
		    int buf_size  	buffer capacity
		    long *file_size_p	file length buffer         

	Returns: error code, or zero if ok

	Calls:	findfirst
		findnext
		strncpy, strcpy, strrchr, strcat    

	Globals: current_path, num_entries, file_block

	Errors:  ERR_SUP_NOENTR    no such entry
*/


int sys_get_entry (char    name_buf[],       /* buffer for entry */
		   int   buf_size,         /* buffer size */
		   long* file_size_p       /* buffer for size value */
		  )


{
	char filename[MAX_PATH_SIZE+1];
	int err;
        int len;
        char *cp;


        /* find next MPX file entry, if any */

        /* if this is the first call */
	if (num_entries==0) {

	       /* construct complete search path name */
	       len = strlen(current_path);
	       if (len==0) strcpy(current_path,"*.MPX");
	       else if (current_path[len-1]=='\\') strcat(current_path,"*.MPX");
	       else strcat(current_path,"\\*.MPX");

	       /* find first entry */
		err = findfirst (current_path,&file_block,0);
        }

        /* otherwise, find next entry */
        else err = findnext(&file_block);

        if (err<0) return(ERR_SUP_NOENTR);

        /* copy filename to buffer */
	filename[buf_size] = NULCH;
	strncpy (filename,file_block.ff_name,
			 (size_t) buf_size);

	/* remove ".MPX" from name */
	cp = strrchr(filename, (int) '.');
        *cp = NULCH;

	/* copy name and size to caller's buffer */
        strcpy (name_buf,filename);
	*file_size_p = file_block.ff_fsize;

        /* count entry */
	num_entries++;

	return(OK);
}

/*
	Procedure: sys_close_dir

	Purpose: close an open directory

	Parameters: none

	Returns: error code; zero if OK

	Calls:   none

	Globals: current_path

	Errors:  none
*/


int sys_close_dir (void)
{
	current_path[0] = NULCH;
	return(OK);
}


/*
	Procedure: sys_check_program

	Purpose: locate and measure a program file

	Parameters: see prototype

	Returns: program size or error code

	Calls:   strlen, strcat
		fopen, fread

	Globals: none

	Errors:  ERR_SUP_NAMLNG    pathname too long
		ERR_SUP_FILNFD    file not found
		ERR_SUP_FILINV    file invalid
*/

#define HEADER_SIZE 32

int sys_check_program(     char     dir_name[],       /* directory name */
			char     prog_name[],      /* program name */
			int*     prog_len_p,       /* ptr to prog length */
			int*     start_offset_p    /* ptr to start offset */
			)
{
        /* buffer for full pathname */
	char file_name[MAX_PATH_SIZE+1];

        /* buffer for file header */
        byte header[HEADER_SIZE];

	FILE *file_p;     /* file pointer for C file routines */
        int num_read;      /* number of header bytes read */
	int file_size;    /* size of complete file */
        int header_size;/* size of header */
        int code_size;     /* size of program code */
        int data_size;     /* size of program data */
        int errcode;       /* error code */


        /* ensure that path name is not too long */
        if ((strlen(dir_name) + strlen(prog_name) + 5)
		 > MAX_PATH_SIZE) return(ERR_SUP_NAMLNG);

	/* construct path name */
	file_name[0] = NULCH;
	if (dir_name[0] != NULCH) {
		strcat(file_name, dir_name);
	        strcat(file_name, "\\");
	}
        strcat(file_name, prog_name);
	strcat(file_name, ".MPX");


        /* try to open the file */
	errcode = OK;
        file_p = fopen(file_name, "rb");
        if (file_p == NULL) return(ERR_SUP_FILNFD);

	/* read the header, check for EXE signature */
        num_read = fread( (void*) header, 1, HEADER_SIZE, file_p);
        if (num_read != HEADER_SIZE) {
		errcode = ERR_SUP_FILINV;
	}
        else if ((header[0] != 0x4D) || (header[1] != 0x5A)) {
		errcode = ERR_SUP_FILINV;
	}

        /* close the file, then return if error occurred */
        fclose(file_p);
	if (errcode!=OK) return(errcode);

        /* compute allocation size needed */
        file_size = (header[5]*256 + header[4] -1) * 512
		+ (header[3]*256 + header[2]);
        header_size =(header[9]*256 + header[8])*16;
        code_size = file_size - header_size;
        data_size = (header[11]*256 + header[10])*16;

        /* return total length and starting offset */
        *prog_len_p = code_size + data_size;
	*start_offset_p = (header[23]*256 + header[22]) * 16;

	return(OK);
}


/*
	Procedure: sys_load_program

	Purpose: load a program file

	Parameters: see prototype

	Returns: error code; zero if OK

	Calls:   intdosx
		segread
		strlen, strcat

	Globals: none 

	Errors:  ERR_SUP_NAMLNG    name too long
		ERR_SUP_FILNFD    file not found
	       ERR_SUP_FILINV      file invalid
		ERR_SUP_PROGSZ    program too large
	       ERR_SUP_LDADDR      address not aligned
		ERR_SUP_LDFAIL    load failed

*/


int sys_load_program (     void*    load_addr,        /* address for loading */
			int      max_size,         /* memory size */
			char     dir_name[],       /* directory name */
			char     prog_name[]       /* program name */
		      )
{

	int errcode;      /* error code */
        int prog_len;      /* program length */
	int offset;       /* offset for starting */
	word norm_seg;    /* normalized load address segment */
        word norm_off;     /* normalized load address offset */

        /* full pathname for file to be loaded */
	char file_name[MAX_PATH_SIZE+1];

        /* parameter block for MSDOS EXEC function */
	struct {
		word load_seg;    /* segment address for loading */
	       word reloc_seg;     /* segment address for relocation */
        }p_block;

	union REGS inregs;            /* input register structure */
	union REGS outregs;           /* output register structure */
	struct SREGS segregs;         /* segment register structure */


        /* check program, get length and offset */
	errcode = sys_check_program(dir_name, prog_name,
					&prog_len, &offset);
        if (errcode < 0) return(errcode);

        /* ensure that allocated memory is large enough */
        if (max_size < prog_len) return(ERR_SUP_PROGSZ);

        /* get normalized segment and offset, check alignment */
	norm_seg = FP_SEG(load_addr);
	norm_off = FP_OFF(load_addr);
	norm_seg += norm_off / 16;
        norm_off %= 16;
	if (norm_off != 0) return(ERR_SUP_LDADDR);

        /* ensure that path name is not too long */
        if ((strlen(dir_name) + strlen(prog_name) + 5)
		 > MAX_PATH_SIZE) return(ERR_SUP_NAMLNG);

	/* construct path name */
	file_name[0] = NULCH;
	if (dir_name[0] != NULCH) {
		strcat(file_name, dir_name);
	        strcat(file_name, "\\");
	}
        strcat(file_name, prog_name);
	strcat(file_name, ".MPX");

        /* get current segment regs */
	segread(&segregs);

        /* setup function codes */
	inregs.h.ah = 0x4B;        /* EXEC function */
	inregs.h.al = 0x03;        /* load only */

        /* setup parameter block and name pointers */
        segregs.ds = FP_SEG(&file_name);
	inregs.x.dx = FP_OFF(&file_name);
        segregs.es = FP_SEG(&p_block);
	inregs.x.bx = FP_OFF(&p_block);

        /* setup load segment -- must be normalized */
	p_block.load_seg = norm_seg;
	p_block.reloc_seg = p_block.load_seg;


        /* call the MS-DOS loader */
	errcode = intdosx(&inregs,&outregs,&segregs);
        errcode = errcode;

        /* check for error */
	if (outregs.x.cflag) return(ERR_SUP_LDFAIL);

	/* load successful */
	return(OK);

}

/* END OF FILE */
