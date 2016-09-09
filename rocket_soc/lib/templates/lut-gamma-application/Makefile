
LUT_ARCH?=default.arch

CROSS_COMPILE?=$(RISCV)/bin/riscv64-unknown-elf-
CLANG?=$(CROSS_COMPILE)gcc


CXX?=clang++
CC?=$(CLANG)

UART?=/dev/ttyUSB1

CFLAGS+= \
  -Iinclude \
  -I$(RISCV)/include \
	-I$(RISCV)/riscv64-unknown-elf/include \
  -std=gnu99 -static -fno-common -fno-builtin-printf \
  -march=RV64IMA
#  -target riscv -mriscv=RV64IAMFD 

CFLAGS= \
  -Iinclude \
  -I$(RISCV)/include \
  -I$(RISCV)/../rocket-soc/rocket_soc/fw/boot/src \
  -I$(RISCV)/../rocket-soc/rocket_soc/fw/common \
  -std=gnu99 -static -fno-common -fno-builtin-printf \
  -march=RV64IMA
LFLAGS+= \
  -L$(RISCV)/lib \
  -lrocket \
	-nostdlib -nostartfiles \
	-T main.ld


# LUT cores to be added to the program
LUTS=lut0

TARGETS=main main.dump main.bin $(LUTS:=.dump)

#LUT_CONFIG_FILES=bitstream.h input_vec.h output_vec.h
# source files
SRC=main.c

# assembly files
S_FILES=lut-startup.s $(patsubst %.c,%.s,$(SRC)) $(LUTS:=.c)

all: $(TARGETS)

lut-debug: $(LUTS:=.debug.pdf)

run: main.bin
	riscv-uart-flash -i main.bin -p $(UART) -w

main: $(S_FILES)
	$(CROSS_COMPILE)gcc $^ -o $@ $(LFLAGS)


main.bin: main
	$(CROSS_COMPILE)objcopy -O binary main main.bin

main.dump: main
	$(CROSS_COMPILE)objdump -D main > main.dump


main.o: main.s
	../../../../../riscv-tools-src/riscv-gnu-toolchain/build/build-binutils-newlib/gas/as-new main.s -o main.o
main.s: $(SRC)
	$(CLANG) $(CFLAGS) -S $(SRC) -o main.s

#$(LUT_CONFIG_FILES): $(LUT_ARCH) 
#	riscv-lut-verify-gen --arch $< -o . --input-size 1000

lut0.c: lut0.input $(LUT_ARCH) 
	riscv-lut-compiler --arch $(LUT_ARCH) -C lut0.input 

lut-startup.s: $(LUT_ARCH)
	riscv-lut-startup -a $(LUT_ARCH) $(LUTS) -o $@

%.s:%.c 
	$(CLANG) $(CFLAGS) -S $^ -o $@

%.debug.pdf:%.input
	riscv-lut-compiler --arch $(LUT_ARCH) -g $^
	gnuplot $*.o.gnuplot

%.lut:%.input
	riscv-lut-compiler --arch $(LUT_ARCH) -i $^ -o $@
%.dump:%.input
	riscv-lut-compiler --arch $(LUT_ARCH) -D $^ -o $@

clean:
	rm -f $(TARGETS) *.S *.o $(S_FILES) $(LUTS:=.lut) $(LUT_CONFIG_FILES) $(LUTS:=.c)
