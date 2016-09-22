#include <rocket/uart.h>
#include <rocket/strutils.h>
#include <inttypes.h>

#pragma paco combine least_precise

// when not using my startup code, the first function to appear is executed.
// We can either have main() be the first one and use forward declarations or
// use a wrapper, as such:
int main_();
int main() { return main_(); }

//#define ALU
#ifdef ALU
  #define approx(...)
#endif

#define IMG_WIDTH 64
#define IMG_HEIGHT 64
#define IMG_RESULT_SIZE (IMG_WIDTH) * (IMG_HEIGHT)

#include "image/data0.h"


uint64_t kernel[9] = {1, 3, 1,
                      3, 9, 3,
                      1, 3, 1,};

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
#define add_approx(idx,arg1,arg2,approx) ({ \                                                                                                                                                                         
  1   uint64_t op1=arg1, op2=arg2,op3=approx rv1; \
  2   asm( \
  3     "add.approx %0, %1, %2, "#idx "\n" \
  4     : "=r"(rv1)  \
  5     : "r"(op1), "r"(op2), "r"(op3)); \
  6   rv1;})
void gauss_alu(){
    int result;
    int value1, value2, value3;
    int intermediate[IMG_RESULT_SIZE];
    int r;
    char buf[128];
    int x, y, i;

    for (x = 1; x < IMG_WIDTH - 1; x++) {
        for (y = 1; y < IMG_HEIGHT - 1; y++) {
            result  = image[(y - 1) * IMG_WIDTH + (x - 1)] * kernel[0];

// inline assembly
     result = add_approx("0",result,image[(y - 1) * IMG_WIDTH + (x    )],"111111") * kernel[1];
            result += image[(y - 1) * IMG_WIDTH + (x    )] * kernel[1];
            result += image[(y - 1) * IMG_WIDTH + (x + 1)] * kernel[2];
            result += image[(y    ) * IMG_WIDTH + (x - 1)] * kernel[3];
            result += image[(y    ) * IMG_WIDTH + (x    )] * kernel[4];
            result += image[(y    ) * IMG_WIDTH + (x + 1)] * kernel[5];
            result += image[(y + 1) * IMG_WIDTH + (x - 1)] * kernel[6];
            result += image[(y + 1) * IMG_WIDTH + (x    )] * kernel[7];
            result += image[(y + 1) * IMG_WIDTH + (x + 1)] * kernel[8];

            result /= 25;
            // print single result to uart
            r=0;
            r+=wruint64(buf+r,result);
            r+=wrstring(buf+r,",");
            uart_println(buf);
        }
    }
}

int main_()
{
    write_raw_image_header();
    gauss_alu();
    uart_println("};");
    uart_exit(0);
    return 0;
}
