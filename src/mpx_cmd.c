#include "mpx_cmd.h"
#include "mpx_util.h"
#include "mpx_supt.h"
#include "mystdlib.h"
#include <string.h>
#include <stdio.h>


/* Strings */

char prompt_str[MAX_LINE]       = "MPX> "; /**<  Prompt sting stores the default Prompt for MPX.   */
char *welcome_message_str       = "\n\n  Welcome to Perpetual Motion Squad's Operating System.\n";  /**< Welocome Message String stores the Welcome Message for MPX. */
char *anykey_str                = "\n<<Press Enter to Continue.>>";/**< Any Key String stores the value of the prompt for the user to press return.*/


/** This function displays the Main Screen for mpx. 
* MPX Command Loop Function dispalays the Main Screen for MPX and functions as the control loop for MPX.
*/
int mpx_command_loop (void) {

	char cmd_line[MAX_LINE];
	char *cmd_argv[MAX_ARGS];
	int  cmd_argc = 0;
	int  i;

	for(;;){ /* infinite loop */
		mpx_cls();
		printf("%s", welcome_message_str);

		/* print menu */
		printf("\n");
		printf("    Main Menu:\n");
		printf("    -----------------------------------------------------------------\n");
		printf("        L    load       Display .MPX files available for loading.\n");
		printf("        D    date       View and set the MPX system date.\n");
		printf("        P    prompt     Change the MPX system prompt.\n");
		printf("        V    version    View version and author information.\n");
		printf("        H    help       Online help.\n");
		printf("        X    exit       Exits the program.\n");
		printf("    -----------------------------------------------------------------\n");
		printf("\n");

		printf("%s", prompt_str);
		mpx_readline(cmd_line, MAX_LINE-1);	

		cmd_argv[0] = strtok(cmd_line, " ");
		cmd_argc++;
		/* cmd_line[] is invalidated after this point; use cmd_argv[][] instead. */

		for(;;){
			cmd_argv[cmd_argc] = strtok(NULL, " ");
			if( cmd_argv[cmd_argc] == NULL ){
				break;
			}
			cmd_argc++;
		}

		switch( cmd_argv[0][0] ) {
			
			case 'l':
			case 'L':
				mpxcmd_load();
			break;
			
			case 'e':
			case 'E':
			case 'x':
			case 'X':
				mpxcmd_exit();
			break;
			
			case 'h':
			case 'H':
			case '?':
				mpxcmd_help();
			break;
			
			case 'p':
			case 'P':
				mpxcmd_prompt();
			break;
			
			case 'd':
			case 'D':
				mpxcmd_date();
			break;
			
			case 'v':
			case 'V':
				mpxcmd_version();
			break;
		
			case '~':
				printf("SECRET DEBUG MENU");
				for (i=0; i<cmd_argc; i++){
					printf("  cmd_argv[%d] = \"%s\"", i, cmd_argv[i]);
				}
				printf("%s", anykey_str); mpxprompt_anykey();
			break;

			default:
				printf("Invalid command.\n");
				printf("%s", anykey_str); mpxprompt_anykey();
			break;
		}
	}
}

/** This function displays the Directory containing the MPX process files.
*/
void mpxcmd_load (void) {
	char buf[10];
	long file_size;

	mpx_cls();
	printf("\n");
	printf("  Contents of MPX Directory (.mpx Files):\n");
	printf("  =======================================\n");
	printf("\n");

	if( sys_open_dir(NULL) != 0 ){
		printf("WARNING: Failed to open MPX directory!\n");
		printf("%s", anykey_str); mpxprompt_anykey();
		return;
	}

	printf("    SIZE        NAME\n");
	printf("    ----------  -------------------------------------------\n");
	while( sys_get_entry(buf, 9, &file_size) == 0 ){
		printf("    %10ld  %s\n", file_size, buf);
	}

	sys_close_dir();

	printf("%s", anykey_str); mpxprompt_anykey();
	return;
}

/** The Help function displays the help for MPX. */
void mpxcmd_help (void) {
	mpx_cls();
	printf("\n");
	printf("  MPX HELP:\n");
	printf(" .. coming soon.\n");

	printf("%s", anykey_str); mpxprompt_anykey();
	return;
}

/** The Version funciton displays MPX version information.*/
void mpxcmd_version (void) {
	mpx_cls();
	printf("\n");
	printf("  ==============================================\n");
	printf("  = MPX System Version R1 - September 17, 2010 =\n");
	printf("  ==============================================\n");
	printf("\n");
	printf("      by the members of PERPETUAL MOTION SQUAD:\n");
	printf("                        --------- ------ -----\n");
	printf("\n");
	printf("             *  Paul Prince  *\n");
	printf("\n");
	printf("                     *  Nicholas Yanak  *\n");
	printf("\n");
	printf("                             *  Nathan Clay  *\n");
	printf("\n");
	printf("\n");
	printf("  WVU Fall 2010 CS450 w/ Lec. Camille Hayhearst\n");

	printf("%s", anykey_str); mpxprompt_anykey();
	return;
}

/** The Prompt function allows the user to change the default prompt. */
void mpxcmd_prompt (void) {
	printf("\n");
	printf("  Current prompt is: \"%s\"\n", prompt_str);
	printf("\n");
	printf("Enter new prompt: ");
	mpx_readline( prompt_str, MAX_LINE );

	printf("%s", anykey_str); mpxprompt_anykey();
	return;
}

/** The Date function allows the user to display or change the date of the MPX system. */
void mpxcmd_date (void) {
	date_rec date;
	sys_get_date(&date);
	printf("\n");
	printf("  System Date:\n");
	printf("    %2d/%2d/%4d\n", date.month, date.day, date.year);
	printf("   (mm/dd/yyyy)\n");
	printf("\n");
	printf("Change it (y/n)? ");
	if( mpxprompt_yn() ) {
		int is_leapyear;
		int max_days;

		printf("\n");

		printf("  New YEAR:  "); date.year	= mpxprompt_int();
		if( !(date.year >=1900 && date.year < 10000) ){
			/* invalid year entered. */
			printf("\nInvalid year entered.\n");
			printf("%s", anykey_str); mpxprompt_anykey();
			return;
		}

		is_leapyear = ((date.year%4==0 && date.year%100!=0)||(date.year%400==0));

		printf("  New MONTH: "); date.month	= mpxprompt_int();

		switch (date.month) {
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				max_days = 31;
			break;

			case 4:
			case 6:
			case 9:
			case 11:
				max_days = 30;
			break;

			case 2:
				if( is_leapyear ) {
					max_days = 29;
				} else {
					max_days = 28;
				}
			break;

			default:
				/* invalid month entered. */
				printf("\nInvalid month entered.\n");
				printf("%s", anykey_str); mpxprompt_anykey();
				return;
			/* break;  commented out to prevent turbo c++ "unreachable code" warning. */
		}

		printf("  New DAY:   "); date.day	= mpxprompt_int();

		if( !(date.day > 0 && date.day <= max_days) ){
			/* invalid day entered. */
			printf("\nInvalid day entered.\n");
			printf("%s", anykey_str); mpxprompt_anykey();
			return;
		}

		/* set the system date. */
		if( sys_set_date(&date) == 0 ){
			printf("Date successfully set!\n");
		} else {
			printf("WARNING:\n");
			printf("sys_set_date() returned error.\n");
			printf("Date may not have been set.\n");
		}
		printf("%s", anykey_str); mpxprompt_anykey();
	}
	return;
}

/** The Exit function allows the user to confirm if they want to exit MPX. */
void mpxcmd_exit (void) {
	printf("\n");
	printf("Are you sure you want to terminate MPX?\n");
	if( mpxprompt_yn() ) {
		printf("EXITING.\n");
		sys_exit();
	}
}
