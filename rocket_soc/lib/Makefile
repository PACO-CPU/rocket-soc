
RISCV?=../../riscv-tools

CROSS_COMPILE?=$(RISCV)/bin/riscv64-unknown-elf-

LIBDIR=lib

CFLAGS+= \
  -Iinclude \
  -std=gnu99 -static -fno-common -fno-builtin-printf \
  -march=RV64IMA

TARGET_LIBS=lib/librocket.a

LIBROCKET_OBJ=$(patsubst %.c,%.o,$(shell find src/*.c))

all: $(TARGET_LIBS)

install: all
	cp -r include/rocket $(RISCV)/include/
	install $(TARGET_LIBS) $(RISCV)/lib/


$(LIBDIR)/librocket.a: $(LIBROCKET_OBJ)
	@mkdir -p lib
	$(CROSS_COMPILE)ar -crv $@ $^

%.o:%.c
	$(CROSS_COMPILE)gcc $(CFLAGS) -c $^ -o $@ 

clean:
	rm -rf $(LIBROCKET_OBJ) $(LIBDIR)
