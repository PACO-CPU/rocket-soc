#include <rocket/uart.h>
#include <rocket/strutils.h>
#include <inttypes.h>
#include <math.h>


// We can either have main() be the first one and use forward declarations or
// use a wrapper, as such:
int main_();
int main() { return main_(); }



#define add_app(mask,arg1,arg2) ({ \
  uint64_t op1=arg1, op2=arg2, rv1; \
  asm( \
    "li x10, %1\n\t" \
    "li x11, %2\n\t" \
    "add.approx %0, x11, x10, "#mask" \n\t" \
    : "=r"(rv1)  \
    : "r"(op1), "r"(op2)); \
  rv1;})

#define mul_app(mask,arg1,arg2) ({ \
  uint64_t op1=arg1, op2=arg2, rv1; \
  asm( \
    "li x10, %1\n\t" \
    "li x11, %2\n\t" \
    "mul.approx %0, x11, x10, "#mask"\n\t" \
    : "=r"(rv1)  \
    : "r"(op1), "r"(op2)); \
  rv1;})


/*


	__asm__ (	"li x10, 100\n\t"
        		"li x11, 150\n\t"
        		"add.approx %0, x11, x10, 60 \n\t"
				:"=r" (result2)); 

*/

int main_() 
{
    char buf[128];
    int i, r, t;
    uint64_t in0;
    uint64_t in1;
    int result;
    print_status_raw();

    uart_println("before mul");
   
    result = mul_app(63,100,100);

    uart_println("after mul");
    r = 0;
    r += wruint64(buf+r, result);
    uart_println(buf);

}



