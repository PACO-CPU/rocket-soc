

CROSS_COMPILE?=$(RISCV)/bin/riscv64-unknown-elf-
GCC?=$(CROSS_COMPILE)gcc

CXX?=clang++
CC?=$(GCC)

UART?=/dev/ttyUSB0
RESULT_IMG=result.png

CFLAGS+= \
  -Iinclude \
  -I$(RISCV)/include \
  -I$(RISCV)/riscv64-unknown-elf/include \
  -std=gnu99 -static -fno-common -fno-builtin-printf \
  -Wattributes \
  -O3 \
  -g3

LFLAGS+= \
  -L$(RISCV)/lib \
  -lrocket \
	-nostdlib -nostartfiles \
	-T main.ld

TARGETS=main main.dump main.bin
#LUT_CONFIG_FILES=bitstream.h input_vec.h output_vec.h
# LUT cores to be added to the program
LUTS=lut0
RAW_IMG_DATA=image/data0.h
IMG_FILE=image/lenna_256x256.png

# source files
SRC=main.c lut0.c 

# assembly files
S_FILES=lut-startup.s $(patsubst %.c,%.s,$(SRC))


all: $(TARGETS)

simulate:
	(cat config/ram_diag.h config/or.h config/and.h config/connection.h)  | \
  sed "s/L,/\n/g" | sed "s/,/\n/g" | sed "s/U//g" | grep -vE "^$$" | \
  ./riscv-lut-tool --arch default.arch -d /dev/stdin -p -s 0 0 0 /dev/stdout

output-config:
	@$(eval CFLAGS := $(CFLAGS) -DOUTPUT)

result-native: clean output-config main.bin
	riscv-uart-flash -w -i main.bin -p $(UART) > result.h
	sed -i '1,6d' result.h
	sed -r -i "s/\x1B\[([0-9]{1,3}((;[0-9]{1,3})*)?)?[m|K]//g" result.h
	./img_raw_converter.py --image -i result.h -o $(RESULT_IMG)
	rm -f result.h

lut-config: 
	@$(eval CFLAGS := $(CFLAGS) -DLUT)
	@echo $(CFLAGS)

result-lut: clean lut-config output-config main.bin
	riscv-uart-flash -w -i main.bin -p $(UART) > result.h
	sed -i '1,6d' result.h
	sed -r -i "s/\x1B\[([0-9]{1,3}((;[0-9]{1,3})*)?)?[m|K]//g" result.h
	./img_raw_converter.py --image -i result.h -o $(RESULT_IMG)
	rm -f result.h

performance-lut: clean lut-config main.bin
	riscv-uart-flash -w -i main.bin -p $(UART)

performance-native: clean main.bin
	riscv-uart-flash -w -i main.bin -p $(UART)

main: $(S_FILES)
	$(GCC) $^ -lm -o $@ $(LFLAGS)

main.bin: main
	$(CROSS_COMPILE)objcopy -O binary main main.bin

main.dump: main
	$(CROSS_COMPILE)objdump -S main > main.dump

main.s: $(SRC) $(RAW_IMG_DATA)
	$(GCC) $(CFLAGS) -S $(SRC) 

lut-startup.s: default.arch
	riscv-lut-startup -a default.arch $(LUTS) -o $@

%.s:%.c 
	$(GCC) $(CFLAGS) -S $^ -o $@

$(RAW_IMG_DATA): $(IMG_FILE)
	./img_raw_converter.py --raw -i $< -o $@


clean:
	rm -f $(TARGETS) *.S *.o $(S_FILES) $(LUT_CONFIG_FILES) $(RAW_IMG_DATA) $(RESULT_IMG)
