#ifndef MPX_UTIL_HFILE
#define MPX_UTIL_HFILE

int	mpx_cls 		(void);
int	mpxprompt_yn		(void);
void	mpx_readline		(char *buffer, int buflen);
char	mpxprompt_anykey	(void);
int	mpxprompt_int		(void);
void	errorDecode		(int err);

#endif