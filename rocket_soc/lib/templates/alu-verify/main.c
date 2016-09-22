#include <rocket/uart.h>
#include <rocket/strutils.h>
#include <inttypes.h>


// when not using my startup code, the first function to appear is executed.
// We can either have main() be the first one and use forward declarations or
// use a wrapper, as such:
int main_();
int main() { return main_(); }

#define xstr(s) str(s)
#define str(s) #s

#define MUL_APPROX(arg1, arg2, amount) ({ \
  uint64_t op1=arg1, op2=arg2, rv1; \
  asm("mul.approx %0, %1, %2, "xstr(amount) "\n" : "=r"(rv1) : "r"(op1), "r"(op2)); \
  rv1;})

#define ADD_APPROX(arg1, arg2, amount) ({ \
  uint64_t op1=arg1, op2=arg2, rv1; \
  asm("add.approx %0, %1, %2, "xstr(amount) "\n" : "=r"(rv1) : "r"(op1), "r"(op2)); \
  rv1;})
#define SUB_APPROX(arg1, arg2, amount) ({ \
  uint64_t op1=arg1, op2=arg2, rv1; \
  asm("sub.approx %0, %1, %2, "xstr(amount) "\n" : "=r"(rv1) : "r"(op1), "r"(op2)); \
  rv1;})

#include "input_data.h"
void alu_test_sub()
{
    int r, i;
    uint64_t a, b, result_precise, result_approx;
    char buf[128];

    uart_println("Running 'sub.approx' test...");
    for(i = 0; i < INPUT_SIZE; i++) {

        a = input_a[i];
        b = input_b[i];

        /* size 2 */
        result_approx = SUB_APPROX(a, b, 63);
        result_precise = (a & ~0x3UL) - (b & ~0x3UL);

        if (result_approx != result_precise) {
            uart_println("Failed 63");
            goto fail;
        }
        /* size 4 */
        result_approx = SUB_APPROX(a, b, 62);
        result_precise = (a & ~((1 << 4) -1)) - (b & ~((1 << 4) -1));

        if (result_approx != result_precise) {
            uart_println("Failed 62");
            goto fail;
        }
        /* size 7 */
        result_approx = SUB_APPROX(a, b, 60);
        result_precise = (a & ~((1 << 7) -1)) - (b & ~((1 << 7) -1));

        if (result_approx != result_precise) {
            uart_println("Failed 60");
            goto fail;
        }
        /* size 10 */
        result_approx = SUB_APPROX(a, b, 56);
        result_precise = (a & ~((1UL << 10) -1)) - (b & ~((1UL << 10) -1));

        if (result_approx != result_precise) {
            uart_println("Failed 56");
            goto fail;
        }
        /* size 15 */
        result_approx = SUB_APPROX(a, b, 48);
        result_precise = (a & ~((1 << 15) -1)) - (b & ~((1 << 15) -1));

        if (result_approx != result_precise) {
            uart_println("Failed 48");
            goto fail;
        }
        /* size 20 */
        result_approx = SUB_APPROX(a, b, 32);
        result_precise = (a & ~((1 << 20) -1)) - (b & ~((1 << 20) -1));

        if (result_approx != result_precise) {
            uart_println("Failed 32");
            goto fail;
        }

        /* size 26 */
        result_approx = SUB_APPROX(a, b, 0);
        result_precise = (a & ~((1 << 26) -1)) - (b & ~((1 << 26) -1));

        if (result_approx != result_precise) {
            uart_println("Failed 0");
            goto fail;
        }

    }
    uart_println("Success");
    return;

fail:
    /* print error */
    r = 0;
    r += wrstring(buf +r, "a = ");
    r += wruint64(buf +r, a);
    r += wrstring(buf +r, "\nb = ");
    r += wruint64(buf +r, b);
    r += wrstring(buf +r, "\nresult_precise = ");
    r += wruint64(buf +r, result_precise);
    r += wrstring(buf +r, "\nresult_approx = ");
    r += wruint64(buf +r, result_approx);
    uart_println(buf);
    return;
}
void alu_test_add()
{
    int r, i;
    uint64_t a, b, result_precise, result_approx;
    char buf[128];

    uart_println("Running 'add.approx' test...");
    for(i = 0; i < INPUT_SIZE; i++) {

        a = input_a[i];
        b = input_b[i];

        /* size 2 */
        result_approx = ADD_APPROX(a, b, 63);
        result_precise = (a & ~0x3UL) + (b & ~0x3UL);

        if (result_approx != result_precise) {
            uart_println("Failed 63");
            goto fail;
        }
        /* size 4 */
        result_approx = ADD_APPROX(a, b, 62);
        result_precise = (a & ~((1 << 4) -1)) + (b & ~((1 << 4) -1));

        if (result_approx != result_precise) {
            uart_println("Failed 62");
            goto fail;
        }
        /* size 7 */
        result_approx = ADD_APPROX(a, b, 60);
        result_precise = (a & ~((1 << 7) -1)) + (b & ~((1 << 7) -1));

        if (result_approx != result_precise) {
            uart_println("Failed 60");
            goto fail;
        }
        /* size 10 */
        result_approx = ADD_APPROX(a, b, 56);
        result_precise = (a & ~((1UL << 10) -1)) + (b & ~((1UL << 10) -1));

        if (result_approx != result_precise) {
            uart_println("Failed 56");
            goto fail;
        }
        /* size 15 */
        result_approx = ADD_APPROX(a, b, 48);
        result_precise = (a & ~((1 << 15) -1)) + (b & ~((1 << 15) -1));

        if (result_approx != result_precise) {
            uart_println("Failed 48");
            goto fail;
        }
        /* size 20 */
        result_approx = ADD_APPROX(a, b, 32);
        result_precise = (a & ~((1 << 20) -1)) + (b & ~((1 << 20) -1));

        if (result_approx != result_precise) {
            uart_println("Failed 32");
            goto fail;
        }

        /* size 26 */
        result_approx = ADD_APPROX(a, b, 0);
        result_precise = (a & ~((1 << 26) -1)) + (b & ~((1 << 26) -1));

        if (result_approx != result_precise) {
            uart_println("Failed 0");
            goto fail;
        }

    }
    uart_println("Success");
    return;

fail:
    /* print error */
    r = 0;
    r += wrstring(buf +r, "a = ");
    r += wruint64(buf +r, a);
    r += wrstring(buf +r, "\nb = ");
    r += wruint64(buf +r, b);
    r += wrstring(buf +r, "\nresult_precise = ");
    r += wruint64(buf +r, result_precise);
    r += wrstring(buf +r, "\nresult_approx = ");
    r += wruint64(buf +r, result_approx);
    uart_println(buf);
    return;
}

void alu_test_mul()
{
    int r, i;
    uint64_t a, b, result_precise, result_approx;
    char buf[128];

    uart_println("Running 'mul.approx' test...");
    for(i = 0; i < INPUT_SIZE; i++) {

        a = input_a[i];
        b = input_b[i];

        /* size 2 */
        result_approx = MUL_APPROX(a, b, 63);
        result_precise = (a & ~0x3UL) * (b & ~0x3UL);

        if (result_approx != result_precise) {
            uart_println("Failed 63");
            goto fail;
        }
        /* size 4 */
        result_approx = MUL_APPROX(a, b, 62);
        result_precise = (a & ~((1 << 4) -1)) * (b & ~((1 << 4) -1));

        if (result_approx != result_precise) {
            uart_println("Failed 62");
            goto fail;
        }
        /* size 7 */
        result_approx = MUL_APPROX(a, b, 60);
        result_precise = (a & ~((1 << 7) -1)) * (b & ~((1 << 7) -1));

        if (result_approx != result_precise) {
            uart_println("Failed 60");
            goto fail;
        }
        /* size 10 */
        result_approx = MUL_APPROX(a, b, 56);
        result_precise = (a & ~((1UL << 10) -1)) * (b & ~((1UL << 10) -1));

        if (result_approx != result_precise) {
            uart_println("Failed 56");
            goto fail;
        }
        /* size 15 */
        result_approx = MUL_APPROX(a, b, 48);
        result_precise = (a & ~((1 << 15) -1)) * (b & ~((1 << 15) -1));

        if (result_approx != result_precise) {
            uart_println("Failed 48");
            goto fail;
        }
        /* size 20 */
        result_approx = MUL_APPROX(a, b, 32);
        result_precise = (a & ~((1 << 20) -1)) * (b & ~((1 << 20) -1));

        if (result_approx != result_precise) {
            uart_println("Failed 32");
            goto fail;
        }

        /* size 26 */
        result_approx = MUL_APPROX(a, b, 0);
        result_precise = (a & ~((1 << 26) -1)) * (b & ~((1 << 26) -1));

        if (result_approx != result_precise) {
            uart_println("Failed 0");
            goto fail;
        }

    }
    uart_println("Success");
    return;

fail:
    /* print error */
    r = 0;
    r += wrstring(buf +r, "a = ");
    r += wruint64(buf +r, a);
    r += wrstring(buf +r, "\nb = ");
    r += wruint64(buf +r, b);
    r += wrstring(buf +r, "\nresult_precise = ");
    r += wruint64(buf +r, result_precise);
    r += wrstring(buf +r, "\nresult_approx = ");
    r += wruint64(buf +r, result_approx);
    uart_println(buf);
    return;
}

int main_()
{
    alu_test_mul();
    alu_test_add();
    alu_test_sub();
    uart_exit(0);
    return 0;
}
