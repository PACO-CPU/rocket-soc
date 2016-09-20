

CROSS_COMPILE?=$(RISCV)/bin/riscv64-unknown-elf-
GCC?=$(CROSS_COMPILE)gcc
CLANG?=$(RISCV)/bin/clang

CXX?=clang++
CC?=clang

UART?=/dev/ttyUSB0
RESULT_IMG=result.png

CFLAGS+= \
  -Iinclude \
  -I$(RISCV)/include \
  -I$(RISCV)/riscv64-unknown-elf/include \
  -std=gnu99  -static -fno-common -fno-builtin-printf \
  -msoft-float \
  -Wattributes \
  #-O3 -g3\
#  -target riscv -mriscv=RV64IAMFD  

LFLAGS+= \
  -L$(RISCV)/lib \
  -lrocket \
	-nostdlib -nostartfiles \
	-T main.ld

TARGETS=main main.dump main.bin
RAW_IMG_DATA=image/data0.h
IMG_FILE=image/image01.png

# source files
SRC=main.c  

# assembly files
S_FILES=$(patsubst %.c,%.s,$(SRC))


all: $(TARGETS)

run: main.bin
	riscv-uart-flash -w -i main.bin -p $(UART) > result.h
	sed -i '1,5d' result.h
	sed -r -i "s/\x1B\[([0-9]{1,3}((;[0-9]{1,3})*)?)?[m|K]//g" result.h
	./img_raw_converter.py --image -i result.h -o $(RESULT_IMG)
	rm -f result.h

run-time: main.bin
	riscv-uart-flash -w -i main.bin -p $(UART)

main: $(S_FILES)
	$(GCC) $^ -lm -o $@ $(LFLAGS)


main.bin: main
	$(CROSS_COMPILE)objcopy -O binary main main.bin

main.dump: main
	$(CROSS_COMPILE)objdump -S main > main.dump

main.s: $(SRC) $(RAW_IMG_DATA)
	$(GCC) $(CFLAGS) -S $(SRC) 

%.s:%.c 
	$(GCC) $(CFLAGS) -S $^ -o $@

$(RAW_IMG_DATA): $(IMG_FILE)
	./img_raw_converter.py --raw -i $< -o $@


clean:
	rm -f $(TARGETS) *.S *.o $(S_FILES) $(LUT_CONFIG_FILES) $(RAW_IMG_DATA) $(RESULT_IMG)
