

CROSS_COMPILE?=$(RISCV)/bin/riscv64-unknown-elf-
CLANG?=$(RISCV)/bin/clang

UART?=/dev/ttyUSB0

CFLAGS+= \
  -Iinclude \
  -I$(RISCV)/include \
  -std=gnu99 -static -fno-common -fno-builtin-printf \
  -march=RV64IMA

LFLAGS+= \
  -L$(RISCV)/lib \
  -lrocket \
	-nostdlib -nostartfiles \
	-T main.ld

TARGETS=main main.dump main.bin

all: $(TARGETS)

run: main.bin
	riscv-uart-flash -i main.bin -p $(UART)

main: main.o
	$(CROSS_COMPILE)gcc $^ -o $@ $(LFLAGS)

main.bin: main
	$(CROSS_COMPILE)objcopy -O binary main main.bin

main.dump: main
	$(CROSS_COMPILE)objdump -D main > main.dump

%.o:%.c
	$(CROSS_COMPILE)gcc $(CFLAGS) -c $^ -o $@ 

clean:
	rm -f $(TARGETS) *.S *.o
