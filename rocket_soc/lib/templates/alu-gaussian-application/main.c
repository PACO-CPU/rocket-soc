#include <rocket/uart.h>
#include <rocket/strutils.h>
#include <inttypes.h>

//#pragma paco combine least_precise

// when not using my startup code, the first function to appear is executed.
// We can either have main() be the first one and use forward declarations or
// use a wrapper, as such:
int main_();
int main() { return main_(); }

#define USE_ADD_APPROX
#define ADD_APPROX_AMOUNT 2

#define USE_MUL_APPROX
#define MUL_APPROX_AMOUNT 2

#include "image/data0.h"

#define xstr(s) str(s)
#define str(s) #s


#ifdef USE_ADD_APPROX
#define ADD_APPROX(arg1, arg2, amount) ({ \
  uint64_t op1=arg1, op2=arg2, rv1; \
  asm("add.approx %0, %1, %2, "xstr(amount) "\n" : "=r"(rv1) : "r"(op1), "r"(op2)); \
  rv1;})
#else
#define ADD_APPROX(arg1, arg2, amount) ({ \
    uint64_t rv1; \
    rv1 = arg1 + arg2;\
    rv1;})
#endif 

#ifdef USE_MUL_APPROX
#define MUL_APPROX(arg1, arg2, amount) ({ \
  uint64_t op1=arg1, op2=arg2, rv1; \
  asm("mul.approx %0, %1, %2, "xstr(amount) "\n" : "=r"(rv1) : "r"(op1), "r"(op2)); \
  rv1;})
#else
#define MUL_APPROX(arg1, arg2, amount) ({ \
    uint64_t rv1; \
    rv1 = arg1 * arg2;\
    rv1;})
#endif 

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

void gauss_alu(){
    int result;
    int image_data;
    int kernel_data;
    int immediate;
    int r;
    char buf[128];
    int x, y, i;

    for (x = 1; x < IMG_WIDTH - 1; x++) {
        for (y = 1; y < IMG_HEIGHT - 1; y++) {

            image_data = image[(y - 1) * IMG_WIDTH + (x - 1)];
            kernel_data = kernel[0];
            result  = MUL_APPROX(image_data, kernel_data, MUL_APPROX_AMOUNT);

            image_data = image[(y - 1) * IMG_WIDTH + (x + 1)];
            kernel_data = kernel[1];
            immediate  = MUL_APPROX(image_data, kernel_data, MUL_APPROX_AMOUNT);
            result = ADD_APPROX(result, immediate, ADD_APPROX_AMOUNT);

            image_data = image[(y   ) * IMG_WIDTH + (x + 1)];
            kernel_data = kernel[ADD_APPROX_AMOUNT];
            immediate  = MUL_APPROX(image_data, kernel_data, MUL_APPROX_AMOUNT);
            result = ADD_APPROX(result, immediate, ADD_APPROX_AMOUNT);

            image_data = image[(y   ) * IMG_WIDTH + (x - 1)];
            kernel_data = kernel[3];
            immediate  = MUL_APPROX(image_data, kernel_data, MUL_APPROX_AMOUNT);
            result = ADD_APPROX(result, immediate, ADD_APPROX_AMOUNT);

            image_data = image[(y   ) * IMG_WIDTH + (x     )];
            kernel_data = kernel[ADD_APPROX_AMOUNT];
            immediate  = MUL_APPROX(image_data, kernel_data, MUL_APPROX_AMOUNT);
            result = ADD_APPROX(result, immediate, ADD_APPROX_AMOUNT);

            image_data = image[(y   ) * IMG_WIDTH + (x + 1)];
            kernel_data = kernel[5];
            immediate  = MUL_APPROX(image_data, kernel_data, MUL_APPROX_AMOUNT);
            result = ADD_APPROX(result, immediate, ADD_APPROX_AMOUNT);

            image_data = image[(y + 1) * IMG_WIDTH + (x - 1)];
            kernel_data = kernel[6];
            immediate  = MUL_APPROX(image_data, kernel_data, MUL_APPROX_AMOUNT);
            result = ADD_APPROX(result, immediate, ADD_APPROX_AMOUNT);

            image_data = image[(y + 1) * IMG_WIDTH + (x    )];
            kernel_data = kernel[7];
            immediate  = MUL_APPROX(image_data, kernel_data, MUL_APPROX_AMOUNT);
            result = ADD_APPROX(result, immediate, ADD_APPROX_AMOUNT);

            image_data = image[(y + 1) * IMG_WIDTH + (x + 1)];
            kernel_data = kernel[8];
            immediate  = MUL_APPROX(image_data, kernel_data, MUL_APPROX_AMOUNT);
            result = ADD_APPROX(result, immediate, ADD_APPROX_AMOUNT);

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
