#include <rocket/uart.h>
#include <inttypes.h>

/** \brief Test application.
  *
  * This will run an echo service on the UART, repeating whatever was written,
  * up to 32 characters.
  */
int main() {
  char buf[32];
  while(1) {
    uart_readln(buf,sizeof(buf));
    uart_println(buf);
  }
  return 0;
}
