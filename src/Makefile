######################
# This is a Makefile #
######################

# Use Turbo C/C++ compiler
CC=tcc
CFLAGS= -C -w
#Debug flags
DFLAGS= -C -w -v



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
	#to change for debug DFALGS insted of CFLAGS

# Clean target (removes build output)
clean:
	clean.bat
