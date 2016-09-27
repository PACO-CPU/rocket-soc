#include <rocket/uart.h>
#include <rocket/axi_maps.h>

#define UART_READ ({ \
  uint32_t status; \
  while(1) { \
    status=uart->status; \
    if (!(status&UART_STATUS_RX_EMPTY)) break; \
  } \
  uart->status=(char)((status>>24)&0xff); \
  (char)((status>>16)&0xff); \
  })

#define UART_WRITE(v) ({ \
  char _v=(v); \
  while(uart->status&UART_STATUS_TX_FULL); \
  uart->data=_v; \
  })

void uart_init() {
  uart_map *uart=(uart_map*)ADDR_NASTI_SLAVE_UART1;
  uart->scaler=304; // 115200 BAUD
}

void uart_waitln() {
  uart_map *uart=(uart_map*)ADDR_NASTI_SLAVE_UART1;
  while (UART_READ!='\n');
}

void uart_write(const char *ptr, size_t cb) {
  uart_map *uart=(uart_map*)ADDR_NASTI_SLAVE_UART1;
  while(cb--) UART_WRITE(*ptr++);
}

void uart_read(char *ptr, size_t cb) {
  uart_map *uart=(uart_map*)ADDR_NASTI_SLAVE_UART1;
  while(cb--) *ptr++=UART_READ;
}

size_t uart_readln(char *ptr, size_t cb_max) {
  uart_map *uart=(uart_map*)ADDR_NASTI_SLAVE_UART1;
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
  uart_map *uart=(uart_map*)ADDR_NASTI_SLAVE_UART1;
  while(*ptr!=0) UART_WRITE(*ptr++);
}
void uart_println(const char *ptr) {
  uart_map *uart=(uart_map*)ADDR_NASTI_SLAVE_UART1;
  while(*ptr!=0) UART_WRITE(*ptr++);
  UART_WRITE('\n');
}

void uart_exit(int status)
{
    char buf[3];
    
    buf[0] = 0;
    buf[1] = status & 0xff;
    buf[2] = '\n';

    uart_write(buf, 3);
    return;
}

static const char hexdig[]="0123456789abcdef";

void uart_write_u64_hex(uint64_t v) {
  uart_map *uart=(uart_map*)ADDR_NASTI_SLAVE_UART1;
  int i;
  for(int i=60;i>=0;i-=4) UART_WRITE(hexdig[(v>>i)&0xf]);
}
void uart_write_u32_hex(uint32_t v) {
  uart_map *uart=(uart_map*)ADDR_NASTI_SLAVE_UART1;
  int i;
  for(int i=28;i>=0;i-=4) UART_WRITE(hexdig[(v>>i)&0xf]);
}
void uart_write_u16_hex(uint16_t v) {
  uart_map *uart=(uart_map*)ADDR_NASTI_SLAVE_UART1;
  int i;
  for(int i=12;i>=0;i-=4) UART_WRITE(hexdig[(v>>i)&0xf]);
}
void uart_write_u8_hex(uint8_t v) {
  uart_map *uart=(uart_map*)ADDR_NASTI_SLAVE_UART1;
  int i;
  for(int i=8;i>=0;i-=4) UART_WRITE(hexdig[(v>>i)&0xf]);
}

void uart_write_int(int64_t v) {
  uart_map *uart=(uart_map*)ADDR_NASTI_SLAVE_UART1;
  char buf[20];
  int n=0;
  if (v<0) {
    UART_WRITE('-');
    v=-v;
  }
  do {
    buf[n++]='0'+v%10;
    v/=10;
  } while (v!=0);
  n--;
  for(;n>=0;n--)
    UART_WRITE(buf[n]);
}
