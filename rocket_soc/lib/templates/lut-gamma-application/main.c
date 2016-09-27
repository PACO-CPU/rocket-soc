#include <rocket/uart.h>
#include <rocket/strutils.h>
#include <rocket/util.h>
#include <inttypes.h>
#include <math.h>
#include "libfixmath/fixmath.h"

#define MODE_RAMP 0
#define MODE_STATIC 1
#define MODE_TIMING 2

#ifndef MODE
#include "mode.h"
#endif

#ifndef CMP_FIXMATH
#define CMP_FIXMATH 1
#endif

#if MODE==MODE_STATIC || MODE==MODE_TIMING
#include "image.h"
#endif

extern const uint64_t lut0[];

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

static const fix16_t fix16_gamma_exp=0x0001fd71;

fix16_t fix16_gamma(fix16_t v) {
  v=fix16_log(v);
  v=fix16_smul(v,fix16_gamma_exp);
  v=fix16_exp(v);
  v<<=8;
  return v;
}

fix16_t fix16_gamma2(fix16_t v, fix16_t gamma) {
  /*
  v=fix16_log(v);
  v=fix16_smul(v,gamma);
  v=fix16_exp(v);
  v<<=8;
  */
  return 0;
  return v;
}

void reconfigure_lut(fix16_t gamma) {
  uint64_t base;
  uint64_t incline;
  fix16_t x0, x1;
  asm("lutl x0, 0,0,0");
  for(int i=0;i<256;i+=2) {
    base=fix16_gamma2(i,gamma);
    incline=fix16_gamma2(i+1,gamma);
    incline=(incline-base)<<10;
    incline=(incline&0xffffffffuL)|(base<<32);
    base=(base>>32)&0xffff;
    asm("lutl %0, 0,0,1"::"r"(incline));
    asm("lutl %0, 0,0,1"::"r"(base));
  }

  for(int i=256;i<552;i++) {
    base=lut0[i];
    asm("lutl %0, 0,0,1"::"r"(base));
  }
}

void reconfigure_fakelut(fix16_t gamma) {
  uint32_t fakelut[256];
  for(int i=0;i<256;i++) {
    fakelut[i]=fix16_gamma2(i,gamma);
  }
}
void reconfigure_computation(fix16_t gamma) {
  for(int i=0;i<256;i++) {
    fix16_gamma2(i,gamma);
  }
}

#if MODE==MODE_STATIC
void process_static() {
  uart_println("computing image..\n");
  uint64_t inp;
  uint64_t res;
  for(size_t i=0;i<sizeof(image)/sizeof(uint32_t);i++) {
    inp=image[i];
    asm("lutw2 zero, zero, 0,1");
    asm("lute %0, %1, 0,0" : "=r"(res) : "r"(inp));
    uart_write_u64_hex(inp);
    uart_print(" ");
    uart_write_u64_hex(res);
    uart_println("");
  }
  uart_println("");
}

void fixmath_process_static() {
  uart_println("computing image (fixmath)..\n");
  fix16_t inp;
  fix16_t res;
  for(size_t i=0;i<sizeof(image)/sizeof(uint32_t);i++) {
    inp=(image[i]%0xffffffff)>>8;
    res=fix16_gamma(inp);
    uart_write_u32_hex(inp);
    uart_print(" ");
    uart_write_u32_hex(res);
    uart_println("");
  }
  uart_println("");
}

#elif MODE==MODE_TIMING
void process_timing(size_t count) {
  uint64_t inp;
  uint64_t res;
  uint64_t t0,t1;

  t0=query_cycle_count();
  asm("lutw2 zero, zero, 0,1");
  for(size_t i=0;i<count;i++) {
    inp=image[i];
    asm("lute %0, %1, 0,0" : "=r"(res) : "r"(inp));
  }
  t1=query_cycle_count();
  uart_print("computation (");
  uart_write_int(count);
  uart_print(" entries with LUT) took: ");
  uart_write_int(t1-t0);
  uart_println(" cycles");
}

void mem_process_timing(size_t count) {
  fix16_t inp;
  fix16_t res;
  uint64_t t0,t1;
  t0=query_cycle_count();
  for(size_t i=0;i<count;i++) {
    
    inp=image[i];
  }
  t1=query_cycle_count();
  uart_print("memory retrievel (");
  uart_write_int(count);
  uart_print(" entries) took: ");
  uart_write_int(t1-t0);
  uart_println(" cycles");
}
void fakelut_process_timing(size_t count) {
  uint64_t inp;
  uint64_t res;
  uint64_t t0,t1;
  char fakelut[0];
  t0=query_cycle_count();
  for(size_t i=0;i<count;i++) {
    
    inp=image[i];
    res=fakelut[(inp>>16)&0xff];
  }
  t1=query_cycle_count();
  uart_print("computation (");
  uart_write_int(count);
  uart_print(" entries with fakelut) took: ");
  uart_write_int(t1-t0);
  uart_println(" cycles");
}

void fixmath_process_timing(size_t count) {
  fix16_t inp;
  fix16_t res;
  uint64_t t0,t1;
  t0=query_cycle_count();
  for(size_t i=0;i<count;i++) {
    
    inp=image[i];
    inp=inp>>8;
    res=fix16_gamma(inp);
  }
  t1=query_cycle_count();
  uart_print("computation (");
  uart_write_int(count);
  uart_print(" entries with fixmath) took: ");
  uart_write_int(t1-t0);
  uart_println(" cycles");
}

void reconf_timing(fix16_t gamma) {
  uint64_t t0,t1;
  t0=query_cycle_count();
  reconfigure_lut(gamma);
  t1=query_cycle_count();
  uart_print("reconfiguration (LUT, gamma=");
  uart_write_int(gamma);
  uart_print(") took: ");
  uart_write_int(t1-t0);
  uart_println(" cycles");
  t0=query_cycle_count();
  reconfigure_fakelut(gamma);
  t1=query_cycle_count();
  uart_print("reconfiguration (fakelut, gamma=");
  uart_write_int(gamma);
  uart_print(") took: ");
  uart_write_int(t1-t0);
  uart_println(" cycles");
  t0=query_cycle_count();
  reconfigure_computation(gamma);
  t1=query_cycle_count();
  uart_print("reconfiguration (computation, gamma=");
  uart_write_int(gamma);
  uart_print(") took: ");
  uart_write_int(t1-t0);
  uart_println(" cycles");
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

void test_fixmath() {
  uart_println("testing fixmath..\n");

  fix16_t v;

  v=fix16_one*234;
  v=fix16_sqrt(v);

  uart_print("sqrt(234) = ");
  uart_write_u32_hex(v);
  uart_println("");


}

void run_test() {
    int i;
    uint64_t word;
    size_t ce_image=sizeof(image)/sizeof(uint32_t);
    uart_println("beginning test..");
    #define TIMING_RAMP size_t count=8;count<=8<<19;count<<=1
    #define RECOMP_RAMP int gamma=5;gamma<200;gamma+=5

    #if MODE==MODE_RAMP
    process_ramp();
    #elif MODE==MODE_STATIC
    process_static();
    #elif MODE==MODE_TIMING
    reconf_timing(0x00003000);
    reconf_timing(0x00006000);
    reconf_timing(0x00009000);
    reconf_timing(0x0000c000);
    reconf_timing(0x0000f000);
    reconf_timing(0x00013000);
    reconf_timing(0x00016000);
    reconf_timing(0x00019000);
    reconf_timing(0x0001c000);
    reconf_timing(0x0001f000);
    for(TIMING_RAMP)
      mem_process_timing(count);
    for(TIMING_RAMP)
      fakelut_process_timing(count);
    for(TIMING_RAMP)
      process_timing(count);
    #endif

    #if CMP_FIXMATH
      #if MODE==MODE_RAMP
      #elif MODE==MODE_STATIC
      fixmath_process_static();
      #elif MODE==MODE_TIMING
      for(TIMING_RAMP)
        fixmath_process_timing(count);
      #endif
    #endif

    
    lut_status();
    
    uart_exit(0);
}


