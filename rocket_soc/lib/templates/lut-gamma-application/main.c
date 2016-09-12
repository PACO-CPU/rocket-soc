#include <rocket/uart.h>
#include <rocket/strutils.h>
#include <inttypes.h>
#include <math.h>

#define MODE_RAMP 0
#define MODE_STATIC 1


#ifndef MODE
#define MODE MODE_RAMP
#endif

#if MODE==MODE_STATIC
#include "image.h"
#endif


void fmt_u64(char *p, uint64_t v);
uint64_t lut_status();
void lut_reset();
void lut_load_config();
void run_test();

int main() { 
  run_test();
  while(1);
  return 0;
}


void fmt_u64(char *p, uint64_t v) {
  static const char alp[]="0123456789abcdef";
  int i;
  for(i=60;i>=0;i-=4) *(p++)=alp[(v>>i)&0xf];
}
uint64_t lut_status() {
    static char s_status[]="status: xxxxxxxxxxxxxxxx  xxxxxxxxxxxxxxxx";
    uint64_t s=0xaffedead;
    asm("luts %0, 0" : "=r"(s));
    fmt_u64(s_status+8,s);
    uart_println(s_status);
    return s;
}
void lut_reset()
{
    asm("lutl x0, 0, 0, 0");
}


#if MODE==MODE_STATIC
void process_static() {
  uart_println("computing image..\n");
  uint64_t inp;
  uint64_t res;
  for(size_t i=0;i<sizeof(image)/sizeof(uint32_t);i++) {
    inp=image[i]<<16;
    asm("lutw2 zero, zero, 0,1");
    asm("lute %0, %1, 0,0" : "=r"(res) : "r"(inp));
    uart_write_u64_hex(inp);
    uart_print(" ");
    uart_write_u64_hex(res);
    uart_println("");
  }
  uart_println("");
}
#endif

void process_ramp() {
  uart_println("computing image..\n");
  uint64_t inp;
  uint64_t res;
  for(size_t i=0;i<1uL<<24;i+=10000) {
    inp=i;
    res=0;
//    asm("lutw2 zero, zero, 0,1");
    asm("lute3 %0, %1, zero, zero, 0" : "=r"(res) : "r"(inp));
    uart_write_u64_hex(inp);
    uart_print(" ");
    uart_write_u64_hex(res);
    uart_println("");
  }
  uart_println("");
}

void run_test() {
    int i;
    uint64_t word;
    uart_println("beginning test..");
    #if MODE==MODE_RAMP
    process_ramp();
    #elif MODE==MODE_STATIC
    process_static();
    #endif
    
    lut_status();

    uart_exit(0);
}


