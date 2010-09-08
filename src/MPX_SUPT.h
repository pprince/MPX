/***********************************************************************
	MPX: The MultiProgramming eXecutive
	Project to Accompany
	A Practical Approach to Operating Systems
	Malcolm G. Lane & James D. Mooney
	Copyright 1993, P.W.S. Kent Publishing Co., Boston, MA.

	File Name:	mpx_supt.h

	Author:	J. Mooney
	Version: 2.0
	Date:  02/24/93

	Purpose: Header file for MPX support

		This file contains support declarations intended
		to be visible to student modules.  It should be
		included by these modules.

		
	Environment: System Independent (header file only)

************************************************************************

 Change Log:

	02/24/92  jdm	original version
	04/06/92  jdm	Module-independent version
	05/15/92  jdm	cleanup and documentation
	07/14/92  jdm	further work
	07/20/92  jdm	cnv flag to int for TURBO C problem
	08/07/92  jdm	changed module codes
        10/18/92  jdm	added load function and errors
	11/17/92  jdm	added memory errors
	12/31/92  jdm	added SET, RESET, and NUM_DEVS
			removed non-ANSI declarations
			changed sys_req params
	01/03/93  jdm	revised error codes and names
	02/24/93  jdm	changed sys_set_vec param to interrupt

************************************************************************/
#ifndef MPX_SUPT
#define MPX_SUPT
/* ANSI C Include Files */
#include <stdio.h>
#include <ctype.h>
#include <string.h>

/* logical constants */
#define	TRUE	1
#define	FALSE	0
#define SET	1
#define RESET	0

#define	NULCH	('\0')

/* status and error codes */
#define OK	0
#define ERR_SUP_INVDEV (-101) /* invalid device id */
#define ERR_SUP_INVOPC (-102) /* invalid op code */
#define ERR_SUP_INVPOS (-103) /* invalid character position */
#define ERR_SUP_RDFAIL (-104) /* read failed */
#define ERR_SUP_WRFAIL (-105) /* write failed */
#define ERR_SUP_INVMEM (-106) /* invalid memory block pointer */
#define ERR_SUP_FRFAIL (-107) /* free failed */
#define ERR_SUP_INVDAT (-108) /* invalid date */
#define ERR_SUP_DATNCH (-109) /* date not changed */
#define ERR_SUP_INVDIR (-110) /* invalid directory name */
#define ERR_SUP_DIROPN (-111) /* directory open error */
#define ERR_SUP_DIRNOP (-112) /* no directory is open */
#define ERR_SUP_NOENTR (-113) /* no more directory entries */
#define ERR_SUP_NAMLNG (-114) /* name too long for buffer */
#define ERR_SUP_DIRCLS (-115) /* directory close error */
#define ERR_SUP_LDFAIL (-116) /* program load failed */
#define ERR_SUP_FILNFD (-117) /* file not found */
#define ERR_SUP_FILINV (-118) /* file invalid */
#define ERR_SUP_PROGSZ (-119) /* program size error */
#define ERR_SUP_LDADDR (-120) /* invalid load address */
#define ERR_SUP_NOMEM  (-121) /* memory allocation error */
#define ERR_SUP_MFREE  (-122) /* memory free error */
#define ERR_SUP_INVHAN (-123) /* invalid handler address */

/* Service operation codes */
#define IDLE	0
#define	READ	1
#define	WRITE	2
#define CLEAR	3
#define GOTOXY	4
#define EXIT	5

/* Device ID codes */
#define NO_DEV		0
#define	TERMINAL	1
#define	PRINTER		2
#define	COM_PORT	3

#define NUM_DEVS	3

/* Module codes */
#define BASE_MASK	0x700F
#define OPT_MASK	0x0FF0

#define MODULE_R1	0x0001
#define MODULE_R2	0x0002
#define MODULE_R3	0x0004
#define MODULE_R4	0x0008
#define MODULE_O1	0x0010
#define MODULE_O2	0x0020
#define MODULE_O3	0x0040
#define MODULE_O4	0x0080
#define MODULE_O5	0x0100
#define MODULE_O6	0x0200
#define MODULE_O7	0x0400
#define MODULE_O8	0x0800
#define MODULE_F	0x4000



/* Type and structure definitions */

typedef int flag;

/* Date record */
typedef struct {
	int month;
	int day;
	int year;
	} date_rec;

/* Support function prototypes */
/* Note that these are all "extern" by default */

	/* sys_init: initialize MPX */
	/*	RETURNS: integer error code; zero if ok */
	int sys_init ( int modules /* module code */
		);

	/* sys_exit: exit to host OS */
	void sys_exit (void);
	
	/* sys_set_vec: set trap vector for system call handler */
	/*	RETURNS: integer error code; zero if ok */
	int sys_set_vec
		( void interrupt (*handler)() /* system call handler */
		);

	/* sys_req: request system service */
	/*	RETURNS: result or error code depending on service */
	int sys_req  ( int op_code,	/* operation code */
			int device_id,	/* device identifier */
			char *buf_p,	/* string buffer */
			int *count_p	/* ptr to transfer count */
		      );

	/* sys_alloc_mem: allocate memory */
	/* RETURNS: pointer to allocated block */
	void *sys_alloc_mem ( size_t size /* block size */
		      );
		      
	/* sys_free_mem: free memory */
	/* RETURNS: integer error code, or 0 if ok */
	int sys_free_mem (	void *ptr /* ptr to memory to free */
			);
		      
	/* sys_get_date: get system date */
	void sys_get_date ( date_rec *date_p /* date record */
		      );

	/* sys_set_date: set system date */
	/*	RETURNS: integer error code, zero if OK */
	int sys_set_date ( date_rec *date_p /* date record */
		      );


	/* sys_open_dir: open a directory */
	/*	RETURNS: integer error code, zero if OK */
	int sys_open_dir (char dir_name[] /* directory name */
		      );
		      
	/* sys_get_entry: get a directory entry */
	/*	RETURNS: integer error code, zero if OK */
	int sys_get_entry (char name_buf[], /* file name buffer */
			   int buf_size, /* name buffer size */
			   long *file_size_p /* file size variable */
			   );
	
	/* sys_close_dir: close a directory */
	/*	RETURNS: integer error code, zero if OK */
	int sys_close_dir (void);

        /* sys_check_program; check and measure program file */
        /* 	RETURNS: memory size needed, or error code */
        int sys_check_program (char dir_name[], /* directory name */
        		char prog_name[], /* program name */
                        int *prog_len, /* program length */
                        int *start_offset /* starting offset */
                        );
		      
	/* sys_load_program: load an MPX program */
	/*	RETURNS: success code or error */
	int sys_load_program (	void *load_addr, /* load address */
        		int max_size, /* maximum program size */
                        char dir_name[], /* directory name */
			char prog_name[] /* program name */
			);

/* END OF FILE */
#endif