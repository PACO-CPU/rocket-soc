#include <rocket/uart.h>
#include <rocket/strutils.h>
#include <inttypes.h>

// when not using my startup code, the first function to appear is executed.
// We can either have main() be the first one and use forward declarations or
// use a wrapper, as such:
int main_();
int main() { return main_(); }
//#define LUT
//#define DEBUG
#define IMG_WIDTH 64
#define IMG_HEIGHT 64
#define IMG_RESULT_SIZE (IMG_WIDTH) * (IMG_HEIGHT)

#define LUTE(idx,arg) ({ \
  uint64_t op1=arg, rv1; \
  asm("lute %0, %1, "#idx ",1\n" : "=r"(rv1) : "r"(op1)); \
  rv1;})

#define LUTE3(idx,arg1,arg2,arg3) ({ \
  uint64_t op1=arg1, op2=arg2, op3=arg3, rv1; \
  asm( \
    "lute3 %0, %1, %2, %3, "#idx "\n" \
    : "=r"(rv1)  \
    : "r"(op1), "r"(op2), "r"(op3)); \
  rv1;})

#include "image/data0.h"


uint64_t kernel[9] = {1, 3, 1,
                      3, 9, 3,
                      1, 3, 1,};

uint64_t get_cycles() {
  uint64_t r;
  asm("rdcycle %0" : "=r"(r));
  return r;
}

void write_raw_image_header()
{
    char buf[64];
    int r = 0;

    r += wrstring(buf + r, "//(");
    r += wruint64(buf + r, IMG_WIDTH-2);
    r += wrstring(buf + r, ",");
    r += wruint64(buf + r, IMG_HEIGHT-2);
    r += wrstring(buf + r, ")\nuint64_t img[] = {");

    uart_println(buf);
}

void gauss_native()
{
    int result, value1, value2, value3;
    int intermediate[IMG_RESULT_SIZE];
    int r;
    char buf[128];
    int x, y, i;

    for (x = 1; x < IMG_WIDTH - 1; x++) {
        for (y = 1; y < IMG_HEIGHT - 1; y++) {
            result  = image[(y - 1) * IMG_WIDTH + (x - 1)] * kernel[0];
            result += image[(y - 1) * IMG_WIDTH + (x    )] * kernel[1];
            result += image[(y - 1) * IMG_WIDTH + (x + 1)] * kernel[2];
            result += image[(y    ) * IMG_WIDTH + (x - 1)] * kernel[3];
            result += image[(y    ) * IMG_WIDTH + (x    )] * kernel[4];
            result += image[(y    ) * IMG_WIDTH + (x + 1)] * kernel[5];
            result += image[(y + 1) * IMG_WIDTH + (x - 1)] * kernel[6];
            result += image[(y + 1) * IMG_WIDTH + (x    )] * kernel[7];
            result += image[(y + 1) * IMG_WIDTH + (x + 1)] * kernel[8];

            result /= 25;
            /* print single result to uart */
            r=0;
            r+=wruint64(buf+r,result);
            r+=wrstring(buf+r,",");
            uart_println(buf);
        }
    }

}

void gauss_lut()
{
    int result, value1, value2, value3;
    int intermediate[IMG_RESULT_SIZE];
    int r;
    char buf[128];
    int x, y, i;
    /* Horizontal */
    for (i = 1; i < (IMG_WIDTH * IMG_HEIGHT)-1; i++) {
        value1= image[i-1];
        value2= image[i];
        value3= image[i+1];
        result = LUTE3(0, value1, value2, value3);

        intermediate[i] = result;
    }

    for (x = 1; x < IMG_WIDTH - 1; x++) {
        for (y = 1; y < IMG_HEIGHT - 1; y++) {
            value1 = intermediate[(y-1) * IMG_WIDTH + x];
            value2 = intermediate[(y  ) * IMG_WIDTH + x];
            value3 = intermediate[(y+1) * IMG_WIDTH + x];
            result = LUTE3(0, value1, value2, value3);
            /* print single result to uart */
            r=0;
            r+=wruint64(buf+r,result);
            r+=wrstring(buf+r,",");
            uart_println(buf);

        }

    }

}

void run_debug()
{
    int x, y, i, r, result;
    char buf[128];

    for (x = 0; x < 256; x = x + 32) {
        for (y = 0; y < 256; y = y + 32) {
            for (i = 0; i < 256; i = i + 32) {
                result = LUTE3(0, x, y, i);

                r = 0;
                r += wrstring(buf + r, "");
                r += wruint64(buf + r, x);
                r += wrstring(buf + r, ",");
                r += wruint64(buf + r, y);
                r += wrstring(buf + r, ",");
                r += wruint64(buf + r, i);
                r += wrstring(buf + r, ",");
                r += wruint64(buf + r, result);
                uart_println(buf);
            }
        }
    }

}

int main_()
{
    char buf[128];
    int r;
    uint64_t cycles = 0;
    write_raw_image_header();
    cycles = get_cycles();
#ifdef LUT
    gauss_lut();
#else
#ifdef DEBUG
    run_debug();
#else
    gauss_native();
#endif
#endif
    uart_println("};");

    cycles = get_cycles() - cycles;
    r = 0;
    r += wrstring(buf + r, "Cycles used = ");
    r += wruint64(buf + r, cycles);
    uart_println(buf);
    uart_exit(0);
    return 0;
}


