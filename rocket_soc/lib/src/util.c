#include <inttypes.h>

uint64_t query_cycle_count() {
  uint64_t r;
  asm("rdcycle %0" : "=r"(r));
  return r;
}

