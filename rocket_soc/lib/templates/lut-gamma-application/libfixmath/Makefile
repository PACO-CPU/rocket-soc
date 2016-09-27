#Project settings
PROJECT = libfixmath
LIB =
SRC = .
INC =

CROSS_COMPILE?=$(RISCV)/bin/riscv64-unknown-elf-

#Compiler settings
CPP = $(CROSS_COMPILE)gcc
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
CPP_FLAGS = -O2 $(INC) -Wall -Wextra -c -march=RV64IMA -g -DFIXMATH_OPTIMIZE_8BIT -DFIXMATH_NO_CACHE
CC_FLAGS  = -O2 $(INC) -Wall -Wextra -c -march=RV64IMA -g -DFIXMATH_OPTIMIZE_8BIT -DFIXMATH_NO_CACHE
AS_FLAGS  = $(CC_FLAGS) -D_ASSEMBLER_
LD_FLAGS = -Wall

# Find all source files
SRC_CPP = $(foreach dir, $(SRC), $(wildcard $(dir)/*.cpp))
SRC_C   = $(foreach dir, $(SRC), $(wildcard $(dir)/*.c))
SRC_S   = $(foreach dir, $(SRC), $(wildcard $(dir)/*.S))
OBJ_CPP = $(patsubst %.cpp, %.o, $(SRC_CPP))
OBJ_C   = $(patsubst %.c, %.o, $(SRC_C))
OBJ_S   = $(patsubst %.S, %.o, $(SRC_S))
OBJ     = $(OBJ_CPP) $(OBJ_C) $(OBJ_S)

# Compile rules.
.PHONY : all
all: $(PROJECT).a

$(PROJECT).a: $(OBJ)
	$(AR) rcs $(PROJECT).a $(OBJ)

$(OBJ_CPP) : %.o : %.cpp
	$(CPP) $(CPP_FLAGS) -o $@ $<

$(OBJ_C) : %.o : %.c
	$(CC) $(CC_FLAGS) -o $@ $<

$(OBJ_S) : %.o : %.S
	$(AS) $(AS_FLAGS) -o $@ $<



# Clean rules
.PHONY : clean
clean:
	rm -f $(PROJECT).a $(OBJ)
