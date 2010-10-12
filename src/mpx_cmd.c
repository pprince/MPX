#include "mpx_cmd.h"
#include "mpx_util.h"
#include "mpx_r2.h"
#include "mpx_supt.h"
#include "mystdlib.h"
#include <string.h>
#include <stdio.h>


/* Strings */

char prompt_str[MAX_LINE]       = "MPX> "; /**<  Prompt sting stores the default Prompt for MPX.   */
char *welcome_message_str       = "\n\n  Welcome to Perpetual Motion Squad's Operating System.\n\n    (type `help commands`) for a list of available commands.)\n\n";  /**< Welocome Message String stores the Welcome Message for MPX. */
char *anykey_str                = "\n<<Press Enter to Continue.>>";/**< Any Key String stores the value of the prompt for the user to press return.*/


mpx_cmd_t *cmd_head = NULL;


void mpx_add_command( char *cmd_name, void(*cmd_function)(int argc, char *argv[]) ) {

	/* allocate a command object */
	mpx_cmd_t *command = (mpx_cmd_t*) sys_alloc_mem( sizeof(mpx_cmd_t) ); /* FIXME need to check for error from alloc func. */

	/* allocate and populate the command name member. */
	command->cmd_name = sys_alloc_mem( strlen(cmd_name)+1 );
	strcpy( command->cmd_name, cmd_name );

	/* populate the command function member. */
	command->cmd_function = cmd_function;

	/* be sure to set the next-command pointer member to NULL, since this will be the new last command. */
	command->next = NULL;

	/* add the command to the global list of commands. */
	if ( cmd_head == NULL ) {
		cmd_head = command;
	} else {
		mpx_cmd_t *last_command = cmd_head;
		while ( last_command->next != NULL ) {
			last_command = last_command->next;
		}
		last_command->next = command;
	}
}

/** This function displays the Main Screen for mpx. 
* MPX Command Loop Function dispalays the Main Screen for MPX and functions as the control loop for MPX.
*/
int mpx_command_loop (void) {

	char cmd_line[MAX_LINE];
	char *cmd_argv[MAX_ARGS+1];
	int  cmd_argc;
	int  i;
	mpx_cmd_t *command;

	mpx_add_command( "help", mpxcmd_help );
	mpx_add_command("load", mpxcmd_load );
	mpx_add_command("date", mpxcmd_date );
	mpx_add_command("exit", mpxcmd_exit );
	mpx_add_command("version", mpxcmd_version );
	mpx_add_command("create",mpxcmd_create_PCB);
	mpx_add_command("delete",mpxcmd_delete_PCB);
	mpx_add_command("block",mpxcmd_block);
	mpx_add_command("unblock",mpxcmd_unblock);
	mpx_add_command("suspend",mpxcmd_suspend);
	mpx_add_command("resume",mpxcmd_resume);
	mpx_add_command("setPriority",mpxcmd_setPriority);
	mpx_add_command("show",mpxcmd_show_PCB);
	mpx_add_command("showAll",mpxcmd_showAll_PCB);
	mpx_add_command("showReady",mpxcmd_showReady_PCB);
	mpx_add_command("showBlocked",mpxcmd_showBlocked_PCB);

	for(;;){ /* infinite loop */

		mpx_cls();

		printf("%s", welcome_message_str);

		printf("%s", prompt_str);

		cmd_argc = 0;

		mpx_readline(cmd_line, MAX_LINE-1);	

		cmd_argv[0] = strtok(cmd_line, " ");
		cmd_argc++;

		/* cmd_line is invalidated after this point; use cmd_argv[] instead. */

		for(i=0; i<MAX_ARGS; i++){
			cmd_argv[cmd_argc] = strtok(NULL, " ");
			if( cmd_argv[cmd_argc] == NULL ){
				break;
			}
			cmd_argc++;
		}

		/* handle too-many-args error condition. */
		if (i == MAX_ARGS && strtok(NULL, " ") != NULL) {
			printf("ERROR: Argument list too long.\n");
			printf("%s", anykey_str); mpxprompt_anykey();
			continue;
		}

		/* run the command function that the user requested,
		 * or print an error message if it is not valid. */
		command = cmd_head;
		while (command != NULL) {
			if ( strcmp(command->cmd_name, cmd_argv[0]) == 0 ) {
				command->cmd_function( cmd_argc, cmd_argv );
				break;
			}
			command = command->next;
		}

		/* if we did not find the requested command in the list of commands,
		 * print an appropriate error message. */
		if ( command == NULL ) {
			 printf("Invalid command.\n");
			 printf("%s", anykey_str); mpxprompt_anykey();
		}
	}
}

/** This function displays the Directory containing the MPX process files.
*/
void mpxcmd_load (int argc, char *argv[]) {
	char buf[10];
	char line_buf[MAX_LINE];
	long file_size;
	int  num_mpx_files = 0;

	mpx_cls();

	if( sys_open_dir(NULL) != 0 ){
		printf("WARNING: Failed to open MPX directory!\n");
		printf("%s", anykey_str); mpxprompt_anykey();
		return;
	}

	mpx_pager_init("  Contents of MPX Directory (.mpx Files):\n  =======================================\n    SIZE        NAME\n    ----------  -------------------------------------------\n");
	while( sys_get_entry(buf, 9, &file_size) == 0 ){
		/* snprintf(&line_buf, MAX_LINE, "    %10ld  %s", file_size, buf); */
		sprintf(&line_buf, "    %10ld  %s", file_size, buf);
		mpx_pager(&line_buf);
		num_mpx_files++;
	}

	sys_close_dir();

	if (num_mpx_files == 0) {
		printf("\n There aren't any .mpx files in the MPX directory!\n\n");
	}

	printf("%s", anykey_str); mpxprompt_anykey();
	return;
}

// this is a user menu funtion designed to give info about other functions takes one or no inputs
void mpxcmd_help(int argc, char *argv[]){
	FILE *fp;
	long fileSize;
	char* buffer;
	char fileName[100];
	size_t data;	
	strcpy(fileName,argv[1]);
	sprintf(buffer,"help\\%s\.txt",fileName);

	
	if(argc==2){ // specific function help
		fp=fopen(buffer,"r");
		fseek(fp,0,SEEK_END);
		fileSize=ftell(fp);
		rewind(fp);
		buffer = (char*) sys_alloc_mem(sizeof(char)*fileSize);
		data = fread (buffer,1,fileSize,fp);
		
		printf("%s",buffer);
	}
	else if(argc==1){ // general help
		fp=fopen("help\\help.txt","r");	
		fseek(fp,0,SEEK_END);
		fileSize=ftell(fp);
		rewind(fp);
		buffer = (char*) sys_alloc_mem(sizeof(char)*fileSize);
		data = fread (buffer,1,fileSize,fp);
		printf("%s",buffer);
	}
	else{
		printf("Wrong number of arguments used or no such command");	
		return;
	}
	fclose(fp);
	printf("%s", anykey_str); mpxprompt_anykey();
	return;
}

/** The Version function displays MPX version information.*/
void mpxcmd_version (int argc, char *argv[]) {
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
void mpxcmd_date (int argc, char *argv[]) {
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
void mpxcmd_exit (int argc, char *argv[]) {
	printf("\n");
	printf("Are you sure you want to terminate MPX?\n");
	if( mpxprompt_yn() ) {
		printf("EXITING.\n");
		sys_exit();
	}
}
