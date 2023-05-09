
# Internal dependencies - Dependencies within our Project
INC = -Iinclude
LIB = -lpthread

# Directories
SRC = src
OBJ = obj
INCLUDE = include
EXT = ext

# Compiler specification
CC = gcc
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG) # compiling flags
LFLAGS = -Wall $(DEBUG) # lib flags

# Add $(SRC) and $(INCLUDE) to search path.
# By doing this, these files do not need explicit path prefixed.
vpath %.c $(SRC)
vpath %.h $(INCLUDE)

# Specify compiling command
MAKE = $(CC) $(INC) -MP -MMD $(args)

# Object files needed by modules
MEM_OBJ = $(addprefix $(OBJ)/, paging.o mem.o cpu.o loader.o common.o)
OS_OBJ = $(addprefix $(OBJ)/, cpu.o mem.o loader.o queue.o os.o sched.o timer.o mm-vm.o mm.o mm-memphy.o common.o)
SCHED_OBJ = $(addprefix $(OBJ)/, cpu.o loader.o common.o)
HEADER = $(wildcard $(INCLUDE)/*.h)









# Build all modules, equivalent with os
all: os

# Compile the whole OS simulation
# into one single executable
os: $(OS_OBJ)
	$(MAKE) $(LFLAGS) $(OS_OBJ) -o os $(LIB)

# Just compile memory management modules
# into one single executable
mem: $(MEM_OBJ)
	$(MAKE) $(LFLAGS) $(MEM_OBJ) -o mem $(LIB)

# Just compile scheduler
# into one single executable
sched: $(SCHED_OBJ)
	$(MAKE) $(LFLAGS) $(MEM_OBJ) -o sched $(LIB)



# Compiling the object files
$(OBJ)/%.o: %.c $(HEADER) $(OBJ)
	$(MAKE) $(CFLAGS) $< -o $@

# Prepare objectives container - create a new folder
$(OBJ):
	mkdir -p $(OBJ)

clean:
	@rm -f $(OBJ)/*.o os sched mem
	@find . -type f -name '*.d'  -delete
	@rm -r $(OBJ)
	@rm -rf _









# Unit-tests rules
test-sample : $(EXT)/munit.c $(EXT)/munit.h 
	@$(MAKE) -o test/sample \
	test/sample.c \
	-Iinclude -I$(EXT) $(EXT)/munit.c

	@./test/sample

test-queue : $(EXT)/munit.c $(EXT)/munit.h
	@$(MAKE) -o test/queue \
	test/queue.c src/common.c src/queue.c \
	-Iinclude -I$(EXT) $(EXT)/munit.c

	@./test/queue

test-sched: $(EXT)/munit.c $(EXT)/munit.h
	@$(MAKE) -o test/sched \
	test/sched.c src/common.c src/sched.c src/queue.c \
	-Iinclude -I$(EXT) $(EXT)/munit.c

	@./test/sched

test-memphy: $(EXT)/munit.c $(EXT)/munit.h
	@$(MAKE) -o test/memphy \
	test/memphy.c src/mm-memphy.c src/common.c \
	-Iinclude -I$(EXT) $(EXT)/munit.c

	@./test/memphy

test-procmem:
	@$(MAKE) -o test/procmem \
	test/procmem.c \
	src/common.c src/mm.c src/mm-memphy.c src/mm-vm.c src/cpu.c \
	-Iinclude

	@./test/procmem

clean-test:
	rm -rf 	test/queue test/sample test/sched \
		  	test/memphy test/procmem
	rm -rf test/*.d
	









# Our own rules
# Please ignore if you are not interested

_ : _.c $(deps)
	@$(MAKE) $(LFLAGS) $^ -o _
	@rm -rf _.d