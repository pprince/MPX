#ifndef MPX_CMD_HFILE
#define MPX_CMD_HFILE


/* Symbolic Constants */ 

#define	MAX_LINE	1024
#define	MAX_ARGS	10


/* Prototypes */

int	mpx_command_loop	(void);
void	mpxcmd_exit		(void);
void	mpxcmd_help		(void);
void	mpxcmd_load		(void);
void	mpxcmd_date		(void);
void	mpxcmd_version		(void);
void	mpxcmd_prompt		(void);


#endif
