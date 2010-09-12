#ifndef MPX_R1_HFILE
#define MPX_R1_HFILE

//functions prepared for pointer call:
int _cdecl exit(char *args);
int _cdecl help(char *args);
int _cdecl version(char *args);
int _cdecl dir(char *args);
int _cdecl date(char *args);
int _cdecl chenv(char *args);



void mpx_command_loop (void);
int mpx_cls (void);
void mpxcmd_exit (void);
int mpxprompt_yn(void);
void mpx_readline ( char *buffer, int buflen );
void mpxcmd_help (void);
char mpxprompt_anykey(void);




#endif
