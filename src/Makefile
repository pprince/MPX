######################
# This is a Makefile #
######################

SHELL=cmd.exe

# Use Turbo C/C++ compiler
CC=tcc
CFLAGS=-C -w
#Debug flags
DFLAGS=-C -w -v

# List of object files (generated; these are NOT in the repo).
OBJS=mpx.obj mpx_supt.obj mpx_util.obj mpx_r2.obj mpx_cmd.obj


# Implicit Rules #
# -------------- #

# Turn .c files into .o (object) files.
%.obj : %.c
	$(CC) $(CFLAGS) -oOBJECT.TMP -c $<
	mv OBJECT.TMP $@


# Explicit Rules #
# -------------- #

mpx.exe: $(OBJS) mpx.c
	$(CC) $(CCFLAGS) -eEXE.EXE $(OBJS)
	mv EXE.EXE $@

# Default make target
all: mpx.exe

# Clean target (removes build output)
clean:
	rm -f  *.obj *.OBJ  *.map *.MAP  *.exe *.EXE  *.tmp *.TMP

