/***********************************************************************
	MPX: The MultiProgramming eXecutive
	Project to Accompany
	A Practical Approach to Operating Systems
	Malcolm G. Lane & James D. Mooney
	Copyright 1993, P.W.S. Kent Publishing Co., Boston, MA.

	File Name:	mpx_cmd.h

	Author:	Nathaniel Clay and Nicholas Yanak
	Version: 1.1
	Date:  12/9/2010

	Purpose: Header for mpx_cmd.c

		
	Environment: Windows XP 32 bit

************************************************************************/
#ifndef MPX_CMD_HFILE
#define MPX_CMD_HFILE


/* Symbolic Constants */ 

#define	MAX_LINE	1024 ///< MAX_LINE defines the maximum size of a line in MPX.
#define	MAX_ARGS	10  ///< MAX_ARGS defines the maximum number of arguments that MPX will accept.


/* Types */

typedef struct mpx_cmd {
	char *cmd_name;  ///< The varable cmd_name holds the name of the command that will be typed in to the MPX command Line.
	struct mpx_cmd *next; ///< The pointer to the next mpx_cmd struct.
	void (*cmd_function)(int argc, char *argv[]);///< Function pointer to the command to be executed by the command Handler.
} mpx_cmd_t;


/* Prototypes */

int		mpx_command_loop(void);
void	mpxcmd_exit		(int argc, char *argv[]);
void	mpxcmd_help		(int argc, char *argv[]);
void	mpxcmd_load		(int argc, char *argv[]);
void	mpxcmd_date		(int argc, char *argv[]);
void	mpxcmd_version	(int argc, char *argv[]);
void	mpxcmd_prompt	(void);


#endif
