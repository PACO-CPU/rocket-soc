#include <rocket/uart.h>
#include <rocket/strutils.h>
#include <inttypes.h>

// when not using my startup code, the first function to appear is executed.
// We can either have main() be the first one and use forward declarations or
// use a wrapper, as such:
int main_();
int main() { return main_(); }

#define IMG_WIDTH 64
#define IMG_HEIGHT 64
#define IMG_RESULT_SIZE (IMG_WIDTH - 2) * (IMG_HEIGHT - 2)

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

/* Define constants as MACRO */


int main_()
{

    int result, value1, value2, value3;
    int x, y, i;
    int intermediate[IMG_RESULT_SIZE];

    int r;
    char buf[128];
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
        }

    }

    r=0;
    r+=wruint64(buf+r,result);
    uart_println(buf);

    uart_readln(buf,sizeof(buf));
    uart_println(buf);

    return 0;
}


