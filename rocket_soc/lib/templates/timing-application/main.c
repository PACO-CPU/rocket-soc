#include <rocket/uart.h>
#include <rocket/strutils.h>
#include <inttypes.h>

uint64_t cycles();
#define CLK_RATE 70000000

/** \brief Test application.
  *
  * This will output the internal clock cycle counter periodically.
  */
int main() {
  char buf[128];
  int r;
  uint64_t t;
  while(1) {
    t=cycles();
    r=0;
    r+=wrstring(buf+r,"cycles:");
    r+=wruint64(buf+r,t);
    r+=wrstring(buf+r," time:");
    r+=wruint64(buf+r,(t*1000)/CLK_RATE);
    uart_println(buf);
  }
  return 0;
}


uint64_t cycles() {
  uint64_t r;
  asm("rdcycle %0" : "=r"(r));
  return r;
}

