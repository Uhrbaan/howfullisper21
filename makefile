PROGRAM=bin/main
CC=gcc

# default flags
# -g 		makes the program debuggable.
# -Wall 	enables all warnings.
# -Iinclude makes the 'include/' folder the root for includes. This means we 
#			can ignore it when including files from the 'include/' folder.
# -O2 		optimisations
ifeq ($(BUILD_TYPE),prod)
	CFLAGS=-O2 -Wall -Iinclude
else
	CFLAGS= -g -Wall -Iinclude
endif

LDFLAGS = # all -L libraries flags

LDLIBS = -lm # all -l libraries

C_FILES := $(shell find src/ -type f -name '*.c') #finds all c files recursively
DEPS := $(patsubst src/%.c, include/%.h, $(C_FILES)) #finds corresponding .h files
OBJS := $(patsubst src/%.c, build/%.o, $(C_FILES))

# Target for default development, build with `$ make`
all: $(PROGRAM)

$(PROGRAM): $(OBJS) main.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) $(OBJS) main.c -o $(PROGRAM)

# generates all the object files for each corresponding .c and .h file.
build/%.o: src/%.c $(DEPS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# To create a production binary, just clean the folder and recompile with the 
# correct C-flags. 
prod:
	$(MAKE) clean
	$(MAKE) BUILD_TYPE=prod all

# removes all the generated files. You might want to run this when there is 
# a compiler error. It will result in a slower build time the first time you 
# re-build the program if you ran `$ make clean` before.
clean:
	rm -r -f bin/* build/*

# Builds the program if there is something to build and the runs the executable.
run: $(PROGRAM)
	@./$(PROGRAM) # run the program

# debug the program using gdb in the command line.
debug:
	@gdb $(PROGRAM)

# for later: implement a target to upload the binary to the chip.

# NOTE:
# Most of this was probably useless :')
# https://docs.zephyrproject.org/latest/develop/application/index.html#build-an-application