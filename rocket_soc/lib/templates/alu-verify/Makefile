

CROSS_COMPILE?=$(RISCV)/bin/riscv64-unknown-elf-
GCC?=$(CROSS_COMPILE)gcc
CLANG?=/home/fpga/pg/paco-env/riscv-tools-src/riscv-llvm/build/Debug+Asserts/bin/clang

CXX?=clang++
CC?=clang

UART?=/dev/ttyUSB1
RESULT_IMG=result.png

CFLAGS+= \
  -Iinclude \
  -I$(RISCV)/include \
  -I$(RISCV)/riscv64-unknown-elf/include \
  -std=gnu99 -static -fno-common -fno-builtin-printf \
  -Wattributes \
  -O0 \
  -g3\

LFLAGS+= \
  -L$(RISCV)/lib \
  -lc \
  -lrocket \
	-nostdlib -nostartfiles \
	-T main.ld

TARGETS=main main.dump main.bin

# source files
SRC=main.c  

# assembly files
S_FILES=$(patsubst %.c,%.s,$(SRC))

INPUT_DATA=input_data.h


all: $(TARGETS)

$(INPUT_DATA):
	./gen_input.py > $@

run: main.bin
	riscv-uart-flash -w -i main.bin -p $(UART) 

run-time: main.bin
	riscv-uart-flash -w -i main.bin -p $(UART)

main: $(S_FILES)
	$(GCC) $^ -lm -o $@ $(LFLAGS)


main.bin: main
	$(CROSS_COMPILE)objcopy -O binary main main.bin

main.dump: main
	$(CROSS_COMPILE)objdump -D main > main.dump

main.s: $(SRC) $(INPUT_DATA)
	$(GCC) $(CFLAGS) -S $(SRC) 

%.s:%.c 
	$(GCC) $(CFLAGS) -S $^ -o $@

$(RAW_IMG_DATA): $(IMG_FILE)
	./img_raw_converter.py --raw -i $< -o $@


clean:
	rm -f $(TARGETS) *.S *.o $(S_FILES) $(LUT_CONFIG_FILES) $(INPUT_DATA) 
