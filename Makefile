
# External dependencies
INC = -Iinclude
LIB = -lpthread

# Directories
SRC = src
OBJ = obj
INCLUDE = include

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
MAKE = $(CC) $(INC) 

# Object files needed by modules
MEM_OBJ = $(addprefix $(OBJ)/, paging.o mem.o cpu.o loader.o)
OS_OBJ = $(addprefix $(OBJ)/, cpu.o mem.o loader.o queue.o os.o sched.o timer.o mm-vm.o mm.o mm-memphy.o)
SCHED_OBJ = $(addprefix $(OBJ)/, cpu.o loader.o)
HEADER = $(wildcard $(INCLUDE)/*.h)









# Build all modules, equivalent with os
all: os

# Compile the whole OS simulation
os: $(OS_OBJ)
	$(MAKE) $(LFLAGS) $(OS_OBJ) -o os $(LIB)

# Just compile memory management modules
mem: $(MEM_OBJ)
	$(MAKE) $(LFLAGS) $(MEM_OBJ) -o mem $(LIB)

# Just compile scheduler
sched: $(SCHED_OBJ)
	$(MAKE) $(LFLAGS) $(MEM_OBJ) -o sched $(LIB)



# Compiling the object files
$(OBJ)/%.o: %.c $(HEADER) $(OBJ)
	$(MAKE) $(CFLAGS) $< -o $@

# Prepare objectives container - create a new folder
$(OBJ):
	mkdir -p $(OBJ)

clean:
	rm -f $(OBJ)/*.o os sched mem
	rm -r $(OBJ)









# Make test
# to-do...









# Our own make rules
# Please contact our Messenger group for further details

_ : _.c $(deps)
	$(MAKE) $(CFLAGS) $^ -o _