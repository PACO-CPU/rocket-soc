#ifndef ROCKET_STRING_H
#define ROCKET_STRING_H

#include <inttypes.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief String composition method appending a string at a given pointer 
  *
  * This will write the content of s to the buffer pointed to at ptr and 
  * terminates it with a null character.
  * This method assumes that the buffer pointed to by ptr is large enough to
  * fit the content of s and the null termination character.
  * \return Number of characters written
  */
int wrstring(char *ptr,const char *s);
/** \brief String composition method appending an integer at a given pointer 
  *
  * This will write the decimal representation of v to the buffer pointed to 
  * by ptr and terminates it with a null character.
  * This method assumes that the buffer pointed to by ptr is large enough to
  * hold the decimal representation (up to 20 characters) and the null
  * termination character.
  * \return Number of characters written
  */
int wruint64(char *ptr,uint64_t v);

/** \brief String composition method appending an integer at a given pointer 
  *
  * This will write a zero-padded hexadecimal representation of v to the buffer 
  * pointed to by ptr and terminates it with a null character.
  * This method assumes that the buffer pointed to by ptr is large enough to
  * hold the hexadecimal representation (16 characters) and the null
  * termination character.
  * \return Number of characters written
  */
void wruint64_hex(char *p, uint64_t v);

#ifdef __cplusplus
}
#endif

#endif
