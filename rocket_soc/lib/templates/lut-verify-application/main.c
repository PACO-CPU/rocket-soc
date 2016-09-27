#include <rocket/uart.h>
#include <rocket/strutils.h>
#include <inttypes.h>
#include <math.h>

#include "input_vec.h"
#include "output_vec.h"

// when not using my startup code, the first function to appear is executed.
// We can either have main() be the first one and use forward declarations or
// use a wrapper, as such:
int main_();
int main() { return main_(); }


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

#define LUTS(idx, arg) do {                  \
    asm("luts %0, "#idx "\n" : "=r"(arg) :); \
} while (0)

typedef struct {
    
    uint64_t reserved_40:40;
    uint16_t cfg_count:16;
    uint8_t reserved_3:3;
    uint8_t state:2;
    uint8_t instr_code:1;
    uint8_t premature_exe:1;
    uint8_t invalid_cfg:1;

} lut_status_t;

typedef union {

    uint64_t U;
    lut_status_t B;
} LutStatus;

static LutStatus getStatus()
{
    LutStatus status;
    LUTS(0, status.U);
    return status;
}

static void print_status_raw()
{
    LutStatus status = getStatus();
    int r = 0;
    char buf[128];
   
    r += wrstring(buf + r, "Raw = ");
    r += wruint64(buf + r, status.U);
    uart_println(buf);
}

static int lut_configure_sucessful()
{
    int fail=0;
    LutStatus status;
    /* load the current status */
    LUTS(0, status.U);

    uart_println("Checking state after configuration ...");
    /* print raw status word */
    print_status_raw();

    switch (status.B.state) {
    case 0:
        uart_println("STATE_RAM_CFG");
        break;
    case 1:
        uart_println("STATE_CHAIN_CFG");
        break;
    case 2:
        uart_println("STATE_READY");
        fail = 0;
        break;
    case 3:
        uart_println("STATE_ERROR:");
        break;
    }
    
    return fail;    
}

int main_() 
{
    char buf[128];
    int i, r;
    uint64_t in0, in1, in2;
    uint64_t result;
    print_status_raw();
    print_status_raw();

    /* sanety check after loading the config */
    if (!lut_configure_sucessful()) {
        r = 0;
        r += wrstring(buf + r, "Starting testrun with ");
        r += wruint64(buf + r, INPUT_SIZE/3);
        r += wrstring(buf + r, " Inputs..."); 
        uart_println(buf);
        for (i = 0; i < INPUT_SIZE/3; i++) {
            /* get the inputs from the input_vec */
            in0 = input_vec[i*3];
            in1 = input_vec[i*3+1];
            in2 = input_vec[i*3+2]; 
            result = LUTE3(0, in0, in1, in2);

            /* compare the result with the expected result from the 
               ht_lut_core.py simulation */ 
            if (result != output_vec[i]) {
                print_status_raw();
                uart_println("Error: expected result != real result");
                r = 0;
                r += wrstring(buf + r, "Expected = ");
                r += wruint64(buf + r, output_vec[i]);
                r += wrstring(buf + r, "\n Got   = ");
                r += wruint64(buf + r, result);
                uart_println(buf);
                goto fail; 
            }
        }

        uart_println("All tests sucessfull");
        uart_exit(0);
    } else {
        /* configuration failed */
        uart_println("Configuration failed.");
    }
fail:
    uart_exit(1);
}



