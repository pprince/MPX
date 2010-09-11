#ifndef MPX_R1_HFILE
#define MPX_R1_HFILE

int printWelcome( void );
int r1( void );
int displayPrompt( void );
int cls(void);
int acceptCommand(char *command);
int analizeCommand( char * comm, char *args);
int load( char *command, int (*PtFunction)(char *args) );
int executeCommand(char *arguments);
//functions prepared for pointer call:
int _cdecl exit(char *args);
int _cdecl help(char *args);
int _cdecl version(char *args);
int _cdecl dir(char *args);
int _cdecl date(char *args);
int _cdecl chenv(char *args);
#endif
