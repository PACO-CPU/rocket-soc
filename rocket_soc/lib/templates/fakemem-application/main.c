#include <rocket/uart.h>
#include <inttypes.h>

int __errno;
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
    uart_println(buf+10*(512<<10));
    if (buf[0]=='q') break;
  }
  uart_exit(12);
  return 0;
}
