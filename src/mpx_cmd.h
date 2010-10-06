#ifndef MPX_CMD_HFILE
#define MPX_CMD_HFILE


/* Symbolic Constants */ 

#define	MAX_LINE	1024
#define	MAX_ARGS	10


/* Types */

typedef struct mpx_cmd {
	char *cmd_name;
	struct mpx_cmd *next;
	void (*cmd_function)(int argc, char *argv[]);
} mpx_cmd_t;


/* Prototypes */

int	mpx_command_loop	(void);
void	mpxcmd_exit		(void);
void	mpxcmd_help		(int argc, char *argv[]);
void	mpxcmd_load		(void);
void	mpxcmd_date		(void);
void	mpxcmd_version		(void);
void	mpxcmd_prompt		(void);


#endif
