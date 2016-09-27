#ifndef ROCKET_UART_H
#define ROCKET_UART_H

#include <string.h>
#include <inttypes.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Initializes the uart for bi-directional communication.
  *
  * This should be called before any uart communication takes place as the
  * BAUD rate might not have been set peforehand.
  */
void uart_init();

/** \brief Reads from the uart until a newline ('\n') character was found.
  */
void uart_waitln();

/** \brief Writes a sequence of bytes onto the uart, blocks until all data
  * was written.
  *
  * \param ptr Pointer into the sequence of bytes to be written.
  * \param cb Number of bytes to write
  */
void uart_write(const char *ptr, size_t cb);

/** \brief Reads a number of bytes from the uart, blocking until all data
  * was received.
  *
  * \param ptr Pointer into the pre-allocated buffer to hold the received data.
  * \param cb Number of bytes to be read. 
  */
void uart_read(char *ptr, size_t cb);

/** \brief Reads a single line from the uart, blocking until all data was
  * received.
  *
  * This method continuously reads from the uart until a newline ('\n') 
  * character was encountered or cb_max-1 bytes were received.
  * The final byte written to the buffer pointed to by ptr is always a 
  * null-terminator.
  * If the reading process was ended by a newline character, it is not 
  * included in the buffer pointed to by ptr.
  *
  * \param ptr Pre-allocated pointer to receive the data.
  * \param cb_max Maximum number of bytes to write into ptr, including 
  * the null-termination character.
  * \return Number of bytes written into ptr, including the terminating 
  * null-terminator.
  */
size_t uart_readln(char *ptr, size_t cb_max);

/** \brief Writes a null-terminated string onto the uart, blocking until all
  * data was written.
  *
  * \param ptr Null-terminated string to be written.
  */
void uart_print(const char *ptr);

/** \brief Writes a null-terminated string onto the uart followed by a newline
  * sequence ("\n"). Blocks until all data was written.
  *
  * \param ptr Null-terminated string to be written.
  */
void uart_println(const char *ptr);

void uart_exit(int status);

void uart_write_u64_hex(uint64_t v);
void uart_write_u32_hex(uint32_t v);
void uart_write_u16_hex(uint16_t v);
void uart_write_u8_hex(uint8_t v);

void uart_write_int(int64_t v);

#ifdef __cplusplus
}
#endif

#endif
