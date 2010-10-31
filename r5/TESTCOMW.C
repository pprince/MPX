/***********************************************************
	MPX: The MultiProgramming eXecutive
	Project to Accompany
	A Practical Approach to Operating Systems
	Malcolm G. Lane & James D. Mooney
	Copyright 1993, P.W.S. Kent Publishing Co., Boston, MA.
	
	File Name: testcomw.c

	Authors: M.G. Lane, J. Mooney
	Version: 2.0
	Date: 10/27/93

	Purpose: Test program for COM port (output only)

	Environments: This file is system independent.
		
	Procedures:	main

	This is a main program which tests the COM driver
	functions com_open, com_write, and com_close.
	

***********************************************************
  Change Log:

	12/29/92  jdm	derived from testprt
	03/07/93  jdm	final version for V2.0b
        10/27/93  jdm	removed IO_COMPLETE

***********************************************************/

#include <dos.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "mpx_supt.h"

#define BUFLEN 100
#define REPEAT_COUNT 5
#define	TIME_LIMIT 5


/* function prototypes */

/* com_open: open the com port */
/*	RETURNS: error code, or zero if ok */
int com_open (int *ef_p, /* ptr to event flag */
		int baud_rate /* requested speed */
	);

/* com_close: close the com port */
/*	RETURNS: error code, or zero if ok */
int com_close (void);

/* com_read: begin block input */
/*	RETURNS: error code, or zero if ok */
int com_read (char *buf_p, /* ptr to buffer */
		int *count_p /* ptr to max. no. of chars to read */
	);

/* com_write: begin block output */
/*	RETURNS: error code, or zero if ok */
int com_write (char *buf_p, /* ptr to buffer */
		int *count_p /* ptr to no. of chars to write */
	);


void abort_test(void);



void main(void)
{
	int e_flag;	/* event flag */
	int ix;		/* loop index */
	int rc;		/* function return code */
	char buffer[BUFLEN]; /* output buffer */
	int length;
	long tstart;

	/* open com port */
	rc = com_open( (int *) &e_flag, 1200);
	if ( rc != 0) {
		printf("\nOPEN failed!\n");
 		printf("error code = %d\n",rc);
		abort_test();
	}

	/* fill buffer with Xs */
	for (ix=0; ix<BUFLEN; ix++) buffer[ix] = 'X';

 	/* insert test string */
 	strcpy(buffer,"This is a test of com driver output  ");
	strcat(buffer,"abcdefghijklmnopqrstuvwxyz 0123456789\012\015");
	length = strlen(buffer);

	/* output test string the specified number of times */
	for (ix=1; ix<=REPEAT_COUNT; ix++) {
		e_flag = 0;
		rc = com_write((char *) buffer, (int *) &length);
		if (rc != 0) {
			printf("\nWRITE error!\n");
			printf("error code = %d\n",rc);
			abort_test();
		}

		/* loop until output is done */
		tstart = time(NULL);
		while (e_flag == 0) {
			if ((time(NULL) - tstart) > TIME_LIMIT) {
				printf("\nTIMEOUT: event flag not set\n");
				abort_test();
			}
		}
        }


	/* output final message */
	e_flag = 0;
	length = 31;
	rc = com_write((char *) "End of Com Driver Output Test\012\015",
				(int *) &length);
	if (rc != 0) {
		printf("\nWRITE error on final message!\n");
		printf("error code = %d\n",rc);
		abort_test();
	}

	/* loop until output is done */
	tstart = time(NULL);
	while (e_flag == 0) {
		if ((time(NULL) - tstart) > TIME_LIMIT) {
			printf("\nTIMEOUT: event flag not set\n");
			abort_test();
		}
	}

	/* close the com port */
	rc = com_close();
	if ( rc != 0) {
		printf("\nCLOSE failed!\n");
 		printf("error code = %d\n",rc);
		exit();
	}

	printf("Test completed successfully!\n");

}


/*
	Abort after error
*/
void abort_test()
{
	int err;

	printf("\nAborting com port test\n");
	err = com_close();
	if (err!=OK) printf("Error on close\n");

	exit();

}

/* END OF FILE */

