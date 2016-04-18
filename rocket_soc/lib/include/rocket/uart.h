#ifndef ROCKET_UART_H
#define ROCKET_UART_H

#include <string.h>
#include <inttypes.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void uart_waitln();
void uart_write(const char *ptr, size_t cb);
void uart_read(char *ptr, size_t cb);
size_t uart_readln(char *ptr, size_t cb_max);
void uart_print(const char *ptr);
void uart_println(const char *ptr);

#ifdef __cplusplus
}
#endif

#endif
