

CROSS_COMPILE?=$(RISCV)/bin/riscv64-unknown-elf-
CLANG?=$(RISCV)/bin/clang

CXX?=clang++
CC?=clang

UART?=/dev/ttyUSB0

CFLAGS+= \
  -Iinclude \
  -I$(RISCV)/include \
	-I$(RISCV)/riscv64-unknown-elf/include \
  -std=gnu99 -static -fno-common -fno-builtin-printf \
  -target riscv -mriscv=RV64IAMFD 
#  -march=RV64IMA

LFLAGS+= \
  -L$(RISCV)/lib \
  -lrocket -lm \
	-nostdlib -nostartfiles \
	-T main.ld

TARGETS=main main.dump main.bin

S_FILES=main.S

all: $(TARGETS)

run: main.bin
	riscv-uart-flash -i main.bin -p $(UART) -w 

main: $(S_FILES)
	$(CROSS_COMPILE)gcc $^ -o $@ $(LFLAGS)


main.bin: main
	$(CROSS_COMPILE)objcopy -O binary main main.bin

main.dump: main
	$(CROSS_COMPILE)objdump -D main > main.dump

%.S:%.c
	$(CLANG) $(CFLAGS) -S $^ -o $@

%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $@ 

clean:
	rm -f $(TARGETS) *.S *.o
