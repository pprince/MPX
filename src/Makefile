######################
# This is a Makefile #
######################

# Use Turbo C/C++
CC=tcc
CFLAGS=-I. -C


# Turn .c files into .o (object) files.
.c.OBJ:
	# -c is the switch for 'compile only'.
	$(CC) $(CFLAGS) -c $<


# List of object files (generated; these are NOT in the repo).
OBJS=MPX_SUPT.OBJ MPX_R1.OBJ MPX.OBJ
EXES=mpx.exe

mpx.exe: $(OBJS)
	$(CC) $(CFLAGS) -e $< $(OBJS)

all: mpx.exe
