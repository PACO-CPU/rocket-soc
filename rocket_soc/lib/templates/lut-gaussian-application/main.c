#include <rocket/uart.h>
#include <rocket/strutils.h>
#include <inttypes.h>

// when not using my startup code, the first function to appear is executed.
// We can either have main() be the first one and use forward declarations or
// use a wrapper, as such:
int main_();
int main() { return main_(); }
//#define LUT
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

void write_raw_image_header()
{
    char buf[64];
    int r = 0;

    r += wrstring(buf + r, "//(");
#ifdef LUT
    r += wruint64(buf + r, IMG_WIDTH-2);
    r += wrstring(buf + r, ",");
    r += wruint64(buf + r, IMG_HEIGHT);
#else
    r += wruint64(buf + r, IMG_WIDTH-2);
    r += wrstring(buf + r, ",");
    r += wruint64(buf + r, IMG_HEIGHT-2);
#endif
    r += wrstring(buf + r, ")\nuint64_t img[] = {");
    
    uart_println(buf);
}

int main_()
{

    int result, value1, value2, value3;
    int x, y, i;
    int intermediate[IMG_RESULT_SIZE];

    int r;
    char buf[128];
    write_raw_image_header(); 
#ifdef LUT
    /* Horizontal */
    for (i = 1; i < IMG_WIDTH * IMG_HEIGHT; i++) {
        value1= image[i-1];
        value2= image[i];
        value3= image[i+1];
        result = LUTE3(0, value1, value2, value3);

        intermediate[i-1] = result;
    }

    for (x = 1; x < IMG_WIDTH; x++) {

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
    r = 0;
    r += wrstring(buf + r, "};");
    uart_println(buf);
#else
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
#endif
    uart_exit(0);
    return 0;
}


