#ifndef MPX_UTIL_HFILE
#define MPX_UTIL_HFILE

void	mpx_pager		(char *line_to_print);
void	mpx_pager_init		(char *header);
int	mpx_cls 		(void);
int	mpxprompt_yn		(void);
void	mpx_readline		(char *buffer, int buflen);
char	mpxprompt_anykey	(void);
int	mpxprompt_int		(void);
void	errorDecode		(int err);

#endif
