# Makefile for to build and execute testcases

# Internal dependencies - Dependencies within our Project
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

