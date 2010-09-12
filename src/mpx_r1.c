#include "mpx_r1.h"
#include "mpx_supt.h"
#include <string.h>
#include <stdio.h>

/* Symbolic Constants */
#define		MAX_CMD_LINE_LENGTH		1024

/* Strings */
char *prompt_str			= "MPX> ";
char *welcome_message_str 		= "Welcome to Perpetual Motion Squad's Operating System.\n";
char *anykey_str			= "\n<<Press Enter to Continue.>>";

void mpx_command_loop (void) {

	char cmd_line[MAX_CMD_LINE_LENGTH] = "";

	for(;;){ /* infinite loop */
		mpx_cls();
		printf("%s", welcome_message_str);

		/* print menu */
		printf("\n");
		printf("Main Menu:\n");
		printf("  exit          Exits the program.\n");
		printf("  help          Online help.\n");
		printf("\n");

		printf("%s", prompt_str);
		mpx_readline(cmd_line, MAX_CMD_LINE_LENGTH);	
		switch( cmd_line[0] ) {
			case 'e':
				mpxcmd_exit();
			break;
			case 'h':
				mpxcmd_help();
			break;
			default:
				printf("Invalid command.\n");
				mpxprompt_yn();
			break;
		}
	}
}

int mpx_cls (void) {
	/* fixme: add error catching */
	int err = sys_req(CLEAR, TERMINAL, NULL, 0);
	
	if ( err != OK ) return err;
	
	return OK;
}

void mpxcmd_help (void) {
	mpx_cls();
	printf("\n  MPX HELP:\n");
	printf(" .. coming soon.\n");

	printf("%s", anykey_str); mpxprompt_anykey();

	return;
}

void mpxcmd_exit (void) {
	printf("Are you sure you want to terminate MPX?\n");
	if( mpxprompt_yn() ) {
		printf("EXITING.\n");
		sys_exit();
	}
}

int mpxprompt_yn(void) {
	char yn = mpxprompt_anykey();
	if( yn == 'Y' || yn == 'y' ) {
		return 1; /* true */
	} else {
		return 0; /* false */
	}
}

char mpxprompt_anykey(void) {
	/* user must press enter. */
	int buflen = 3;
	char buf[5];
	buf[0] = ' ';
	sys_req(READ, TERMINAL, buf, &buflen);
	return buf[0];
}

void mpx_readline ( char *buffer, int buflen ) {
	int local_buflen = buflen;
	int err = sys_req(READ, TERMINAL, buffer, &local_buflen);
}

