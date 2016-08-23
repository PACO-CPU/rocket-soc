

CROSS_COMPILE?=$(RISCV)/bin/riscv64-unknown-elf-
CLANG?=$(CROSS_COMPILE)gcc

CXX?=clang++
CC?=$(CLANG)

UART?=/dev/ttyUSB0

CFLAGS+= \
  -Iinclude \
  -I$(RISCV)/include \
	-I$(RISCV)/riscv64-unknown-elf/include \
  -std=gnu99 -static -fno-common -fno-builtin-printf \
  #-target riscv -mriscv=RV64IAMFD 
#  -march=RV64IMA

LFLAGS+= \
  -L$(RISCV)/lib \
  -lrocket \
	-nostdlib -nostartfiles \
	-T main.ld

TARGETS=main main.dump main.bin
LUT_CONFIG_FILES=bitstream.h input_vec.h output_vec.h
# LUT cores to be added to the program
LUTS=lut0

# source files
SRC=main.c $(LUTS:=.c)

# assembly files
S_FILES=lut-startup.s $(patsubst %.c,%.s,$(SRC))


all: $(TARGETS)

run: main.bin
	riscv-uart-flash -i main.bin -p $(UART)

main: $(S_FILES)
	$(CROSS_COMPILE)gcc $^ -o $@ $(LFLAGS)


main.bin: main
	$(CROSS_COMPILE)objcopy -O binary main main.bin

main.dump: main
	$(CROSS_COMPILE)objdump -D main > main.dump


main.s: $(SRC) $(LUT_CONFIG_FILES)
	$(CLANG) $(CFLAGS) -S $(SRC) 

$(LUT_CONFIG_FILES): default.arch
	riscv-lut-verify-gen --arch $< -o . --input-size 50

#lut0.c: lut0.input default.arch
#	riscv-lut-compiler --arch default.arch -C lut0.input

lut-startup.s: default.arch
	riscv-lut-startup -a default.arch $(LUTS) -o $@

%.s:%.c 
	$(CLANG) $(CFLAGS) -S $^ -o $@

clean:
	rm -f $(TARGETS) *.S *.o $(S_FILES) $(LUTS:=.lut) $(LUT_CONFIG_FILES)
