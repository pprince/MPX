######################
# This is a Makefile #
######################

# Use Turbo C/C++ compiler
CC=tcc
CFLAGS= -C -w

# List of object files (generated; these are NOT in the repo).
OBJS=MPX_SUPT.OBJ MPX_R1.OBJ MPX.OBJ

# Default make target
all: mpx.exe


# Explicit Rules #
# -------------- #

mpx.exe: $(OBJS)
	$(CC) $(CCFLAGS) -e$< $(OBJS)


# Implicit Rules #
# -------------- #

# Turn .c files into .o (object) files.
.c.OBJ:
	# -c is the switch for 'compile only'.
	$(CC) $(CFLAGS) -c $<


# Clean target (removes build output)
clean:
	rm -f *.OBJ
	rm -f *.obj
	rm -f *.EXE
	rm -f *.exe
	rm -f *.MAP
	rm -f *.map
