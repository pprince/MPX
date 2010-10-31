/***********************************************************
	MPX: The MultiProgramming eXecutive
	Project to Accompany
	A Practical Approach to Operating Systems
	Malcolm G. Lane & James D. Mooney
	Copyright 1993, P.W.S. Kent Publishing Co., Boston, MA.
	
	File Name: testcomr.c

	Authors: M.G. Lane, J. Mooney
	Version: 2.1b
	Date: 10/27/93

	Purpose: Test program for COM port (input & output)

	Environments: This file is system independent.
		
	Procedures:	main

	This is a main program which tests the COM driver
	functions com_open, com_read, com_write, and com_close.


***********************************************************
  Change Log:

	12/29/92  jdm	derived from testcomw
	03/08/93  jdm	final version for V2.0b
        10/27/93  jdm	removed IO_COMPLETE and far refs

***********************************************************/

#include <dos.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "mpx_supt.h"

#define BUFLEN 100
#define REPEAT_COUNT 5
#define	WR_TIME_LIMIT 5
#define RD_TIME_LIMIT 50

/* driver function prototypes */

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


/* auxiliary function prototypes */
void abort_test(void);



void main()
{
	int e_flag;	/* event flag */
	int rc;		/* function return code */
	char buffer[BUFLEN+2];	/* general buffer */
	char prompt[BUFLEN+2];	/* prompt buffer */
	int prlen;
	int length;
	long tstart;

	/* open com port */
	rc = com_open( (int*) &e_flag, 1200);
	if ( rc != 0) {
		printf("\nOPEN failed!\n");
 		printf("error code = %d\n",rc);
		abort_test();
	}


	/* setup prompt string */
	strcpy(prompt, "\r\nEnter string: ");
	prlen = strlen(prompt);

	/* prompt and read until "quit" is typed */
	buffer[0] = NULCH;
	while (strcmp(buffer,"quit\r\n") != 0) {

		/* display the prompt */
		e_flag = 0;
		rc = com_write((char*) prompt, (int*) &prlen);
		if (rc != 0) {
			printf("\nerror displaying prompt!\n");
			printf("error code = %d\n",rc);
			abort_test();
		}

		/* loop until output is done */
		tstart = time(NULL);
		while (e_flag == 0) {
			if ((time(NULL) - tstart) > WR_TIME_LIMIT) {
				printf("\ntimeout on prompt\n");
				printf("event flag not set\n");
				exit();
			}
		}

		/* read a string */
		length = BUFLEN-1;
		e_flag = 0;
		rc = com_read((char*) buffer, (int*) &length);
		if (rc != 0) {
			printf("\nerror reading string!\n");
			printf("error code = %d\n",rc);
			abort_test();
		}

		/* loop until input is done */
		tstart = time(NULL);
		while (e_flag == 0) {
			if ((time(NULL) - tstart) > RD_TIME_LIMIT) {
				printf("\ntimeout on input\n");
				printf("event flag not set\n");
				abort_test();
			}
		}


		/* display the input string */
		e_flag = 0;
                if (buffer[length-1] == '\n') {
                   buffer[length-1] = NULCH;
                   length--;
                }
                strcat(buffer,"\r\n");
                length = length + 2;

		printf("%s",buffer);

		rc = com_write((char*) buffer, (int*) &length);
		if (rc != 0) {
			printf("\nerror displaying string!\n");
			printf("error code = %d\n",rc);
			abort_test();
		}

		/* loop until output is done */
 		tstart = time(NULL);
		while (e_flag == 0) {
			if ((time(NULL) - tstart) > WR_TIME_LIMIT) {
				printf("\ntimeout on output\n");
				printf("event flag not set\n");
				abort_test();
			}
		}

	} /* end of while loop */

	/* print final message */
	e_flag = 0;
	length = 29;
	printf("\nEnd of Com Driver IO Test\n");
	rc = com_write((char*) "\r\nEnd of Com Driver IO Test\r\n",
				(int*) &length);
	if (rc != 0) {
		printf("\nWRITE error on final message!\n");
		printf("error code = %d\n",rc);
		abort_test();
	}

	/* loop until output is done */
 	tstart = time(NULL);
	while (e_flag == 0) {
		if ((time(NULL) - tstart) > WR_TIME_LIMIT) {
			printf("\ntimeout on final message\n");
			printf("event flag not set\n");
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
void abort_test(void)
{
	int err;

	printf("\nAborting com port test\n");
	err = com_close();
	if (err!=OK) printf("Error on close\n");

	exit();

}

/* END OF FILE */

