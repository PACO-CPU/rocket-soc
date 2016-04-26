#include <rocket/uart.h>
#include <rocket/axi_maps.h>

static uart_map *_uart=(uart_map*)ADDR_NASTI_SLAVE_UART1;

#define UART_READ ({ \
  uint32_t status; \
  while(1) { \
    status=_uart->status; \
    if (!(status&UART_STATUS_RX_EMPTY)) break; \
  } \
  _uart->status=(char)((status>>16)&0xff); \
  (char)((status>>24)&0xff); \
  })

#define UART_WRITE(v) ({ \
  char _v=(v); \
  while(_uart->status&UART_STATUS_RX_EMPTY); \
  _uart->data=v; \
  })

void uart_init() {
  _uart->scaler=304; // 115200 BAUD
}

void uart_waitln() {
  while (UART_READ!='\n');
}

void uart_write(const char *ptr, size_t cb) {
  while(cb--) UART_WRITE(*ptr++);
}

void uart_read(char *ptr, size_t cb) {
  while(cb--) *ptr++=UART_READ;
}

size_t uart_readln(char *ptr, size_t cb_max) {
  int cr=0;
  size_t n=0;
  if (cb_max<1) return 0;
  while(n++<cb_max-1) {
    if ((*ptr++=UART_READ)=='\n') {
      ptr--;
      break;
    }
  }
  *ptr=0;
  return n;
}
void uart_print(const char *ptr) {
  while(*ptr!=0) UART_WRITE(*ptr++);
}
void uart_println(const char *ptr) {
  while(*ptr!=0) UART_WRITE(*ptr++);
  UART_WRITE('\n');
}

