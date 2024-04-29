/*
 * Copyright (C) Narf Industries <info@narfindustries.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef LIBC_H
#define LIBC_H

#include "libcgc.h"
#include "cgc_stdint.h"
#include "cgc_prng.h"

#define MAGICWORD "Please? Thanks? Sorry?"
#define MEMERR "Have you ever heard of the old robot saying, 'DOES NOT COMPUTE'?"
#define RNDERR "Ten. Three. Twelve. Three again. Fifteen. Negative eight. Twenty seven. Ninety one. Hey, zero!"
#define COOKIEFAIL "You know what really cheers me up? Baking a tray of sugar cookies."

/**
 * Gets size of struct member
 *
 * @param t Struct type
 * @param m Member to check
 * @return Size of structure
 */
#define MEMBERSIZE(t,m) sizeof(((t*)0)->m)

/**
 * Sends a buffer and terminates on failure.
 *
 * @param s Size of the buffer
 * @param b Buffer
 * @return Number of bytes sent
 */
#define SSEND(s,b) if(cgc_sendall(STDOUT,b,s)<0)  cgc__terminate(3);

/**
 * Sends a buffer+newline and terminates on failure.
 *
 * @param s Size of the buffer
 * @param b Buffer
 * @return Number of bytes sent
 */
#define SSENDL(s,b) if(cgc_sendline(STDOUT,b,s)<0) cgc__terminate(6);

/**
 * Sends a buffer+newline and terminates on failure.
 *
 * @param b Buffer
 * @return Number of bytes sent
 */
#define LOG(b) SSENDL(sizeof(b)-1,b);

/**
 * Sends an error message and terminates.
 *
 * @param b Buffer
 * @param c Error code
 */
#define TERM(b,c)  LOG(b); cgc__terminate(c);

/**
 * Receives a newline terminated string into buffer, and replaces newline with NULL
 * Terminates on error
 *
 * @param s Size of the buffer
 * @param b Buffer
 * @return Data in buffer
 */
#define SRECV(s,b) if(cgc_recvline(STDIN,b,s)<0){SSENDL(sizeof(MAGICWORD)-1,MAGICWORD); cgc__terminate(2);}

/**
 * Receives bytes into buffer
 * Terminates on error
 *
 * @param s Number of bytes to cgc_read into the buffer
 * @param b Buffer
 * @return Data in buffer
 */
#define RECV(s,b) if(cgc_recv(STDIN,b,s)<0){SSENDL(sizeof(MAGICWORD)-1,MAGICWORD); cgc__terminate(4);}

/**
 * Read in data from cgc_stdin to fixed size type
 *
 * @param s buffer to cgc_read into
 * @return 0 on success, 1 on failure
 */
#define READDATA(s) (sizeof(s) == cgc_recv(STDIN,(char *)&s,sizeof(s)) ? 0 : 1)

/**
 * Thin wrapper to allocate
 * Terminates on error
 *
 * @param x Is executable
 * @param a Location to store address
 * @param s Size of allocation
 * @return Address in a
 */
#define ALLOC(x,a,s) if(cgc_allocate(s,x,a)!=0){ SSENDL(sizeof(MEMERR)-1,MEMERR); cgc__terminate(9);}

/**
 * Thin wrapper to deallocate
 * Terminates on error
 *
 * @param a Address to deallocate
 * @param s Size of allocation
 */
#define DEALLOC(a,s) if(deallocate(a,s)!=0){ SSENDL(sizeof(MEMERR)-1,MEMERR); cgc__terminate(18);}

/**
 * Thin wrapper to rand, our random replacement
 * Terminates on error
 *
 * @param b Buffer to store random data
 * @param s Count of random bytes to cgc_read
 * @param r Location to store number of bytes cgc_read
 * @return Random data in b
 */
#define RAND(b,s,r) if (cgc_rand(b,s,r)){ SSENDL(sizeof(RNDERR)-1,RNDERR); cgc__terminate(19);}

#define STACKPROTECTINIT extern uint32_t cgc___cookie;
#define STACKPROTECTADD  uint32_t __wat = cgc___cookie;
#define STACKPROTECTCHK  if ( (__wat = __wat ^ cgc___cookie) != 0 ) cgc___stack_cookie_fail();

#define DEBUG 1
#ifdef DEBUG
#define debug(s) cgc_sendall(2,s,cgc_strlen(s))
#else
#define debug(args...)
#endif

typedef enum {
    YOLO,
    SHEAP,
    SSHEAP
} heaptype;

extern uint32_t cgc___hcookie[3];

#define PAGE_SIZE 4096
#define FLAG_PAGE CGC_FLAG_PAGE_ADDRESS
typedef struct heap_chunk heap_chunk_t;

struct heap_chunk {
    uint32_t cookie;
    heap_chunk_t *prev;
    heap_chunk_t *next;
    uint32_t size;
};

/**
 * Simple prompt handler
 *
 * @param buf Buffer to store response
 * @param s Size of response buffer
 * @param prompt Prompt to send
 * @return Response data in buf
 */
void cgc_promptc(char *buf, uint16_t  size, char *prompt);

/**
 * Convert unsigned integer to string
 *
 * @param str_buf Destination buffere
 * @param buf_size Destination buffer size
 * @param i Integer to convert
 * @return Ascii-representation of i in str_buf
 */
int cgc_uint2str(char* str_buf, int buf_size, uint32_t i);

/**
 * Convert signed integer to string
 *
 * @param str_buf Destination buffere
 * @param buf_size Destination buffer size
 * @param i Integer to convert
 * @return Ascii-representation of i in str_buf
 */
int cgc_int2str(char* str_buf, int buf_size, int i);

/**
 * Convert string to signed integer
 *
 * @param str_buf Source buffer
 * @return integer
 */
uint32_t cgc_str2uint(const char* str_buf);

/**
 * Send bytes from buffer to file descriptor
 *
 * @param fd File descriptor to send on
 * @param buf Source buffer
 * @param size Number of bytes to send
 * @return Number of bytes sent, -1 on error
 */
int cgc_sendall(int fd, const char *buf, cgc_size_t size);

/**
 * Send bytes from buffer to file descriptor with newline
 *
 * @param fd File descriptor to send on
 * @param buf Source buffer
 * @param size Number of bytes to send
 * @return Number of bytes sent, -1 on error
 */
int cgc_sendline(int fd, const char *buf, cgc_size_t size);

/**
 * Receive line from file descriptor
 *
 * @param fd File descriptor to receive on
 * @param buf Destination buffer
 * @param size Size of destination buffer
 * @return Number of bytes received, -1 on error
 */
int cgc_recvline(int fd, char *buf, cgc_size_t size);

/**
 * Receive bytes from file descriptor
 *
 * @param fd File descriptor to receive on
 * @param buf Destination buffer
 * @param size Number of bytes to receive
 * @return Number of bytes received, -1 on error
 */
int cgc_recv(int fd, char *buf, cgc_size_t size);

/**
 * Copy a string
 *
 * @param s1 Destination buffer
 * @param s2 Source buffer
 * @return Number of bytes copied
 */
cgc_size_t cgc_strcpy(char *s1, const char *s2);

/**
 * Copy a string with bounds checking
 *
 * @param s1 Destination buffer
 * @param s2 Source buffer
 * @param n Size of destination buffer
 * @return Number of bytes copied
 */
cgc_size_t cgc_strncpy(char *s1, char *s2, cgc_size_t n);

/**
 * Concatenate two strings
 *
 * @param s1 Main string
 * @param s2 String to be concatenated
 * @return s1
 */
char * cgc_strcat(char *s1, char *s2);

/**
 * Find length of string
 *
 * @param s String
 * @return length of s
 */
cgc_size_t cgc_strlen(char *s);

/**
 * Check if two strings are identical
 *
 * @param s1 String 1
 * @param s2 String 2
 * @return 1 if identical, 0 if different
 */
int cgc_streq(char *s1, char *s2);

/**
 * Compare two strings
 *
 * @param s1 String 1
 * @param s2 String 2
 * @return 0 if identical, 1 if different
 */
int cgc_strncmp(char *s1, char *s2, cgc_size_t n);

/**
 * Check if a string starts with another string
 *
 * @param s1 String to check
 * @param s2 String that s1 might start with
 * @return 1 if s1 starts with s2, 0 if not
 */
int cgc_startswith(char *s1, char *s2);

/**
 * Set a buffer to provided character
 *
 * @param dst Destination buffer
 * @param c Character to repeat
 * @param n Number of times to copy character
 * @return dst
 */
void * cgc_memset(void *dst, char c, cgc_size_t n);

/**
 * Copy bytes from one buffer to another
 *
 * @param dst Destination buffer
 * @param src Source buffer
 * @param n Number of bytes to copy
 * @return dst
 */
void * cgc_memcpy(void *dst, void *src, cgc_size_t n);

/**
 * Convert byte to hex character string
 *
 * @param b Byte to convert
 * @param h Destination hex string
 * @return h
 */
char * cgc_b2hex(uint8_t b, char *h);

/**
 * Locate character in string
 *
 * @param str String to search
 * @param h Character to find
 * @return Pointer to character in string, or NULL
 */
char * cgc_strchr(char *str, char c);

/**
 * Sleep process
 *
 * @param s Amount of time to sleep
 */
void cgc_sleep(int s);

/**
 * Compare two buffers
 *
 * @param a First buffer
 * @param b Second buffer
 * @param n Number of bytes to compare
 * @return -1 if not equal, 0 if equal
 */
int cgc_memcmp(void *a, void *b, cgc_size_t n);

/**
 * Allocate a buffer on simple heap
 *
 * @param size Size of buffer to allocate
 * @return Pointer to newly allocated buffer
 */
void *cgc_nmalloc(cgc_size_t size);

/**
 * Allocate a buffer on secure heap
 *
 * @param size Size of buffer to allocate
 * @return Pointer to newly allocated buffer
 */
void *cgc_smalloc(cgc_size_t size);

/**
 * Allocate a buffer on super secure heap
 *
 * @param size Size of buffer to allocate
 * @return Pointer to newly allocated buffer
 */
void *cgc_ssmalloc(cgc_size_t size);

/**
 * Free an allocated buffer on simple heap
 *
 * @param p Pointer to buffer
 */
void cgc_nfree(void *p);

/**
 * Free an allocated buffer on secure heap
 *
 * @param p Pointer to buffer
 */
void cgc_sfree(void *p);

/**
 * Free an allocated buffer on super secure heap
 *
 * @param p Pointer to buffer
 */
void cgc_ssfree(void *p);

/**
 * Free an allocated buffer
 *
 * @param p Pointer to buffer
 */
void cgc_free(void *p);

/**
 * Allocate a buffer on heap
 *
 * @param size Size of buffer to allocate
 * @return Pointer to newly allocated buffer
 */
void *cgc_malloc(cgc_size_t size);

/**
 * Allocate a zeroed buffer on heap
 *
 * @param size Size of buffer to allocate
 * @return Pointer to newly allocated buffer
 */
void *cgc_calloc(cgc_size_t size);

/**
 * Configure the heap type to use
 *
 * @param type Type of heap
 */
void cgc_setheap(uint32_t type);

/**
 * Initialize stack cookie
 *
 */
void cgc___stack_cookie_init();

/**
 * Check stack cookie
 *
 */
void cgc___stack_cookie_fail();

/**
 * Check heap cookie
 *
 */
void cgc___heap_cookie_fail();

/**
 * Check heap cookie if enabled, return next chunk.
 * Terminates on error
 *
 * @param a current heap_chunk_t
 * @param type heap type
 * @return a->next if safe, terminate if not
 */
inline heap_chunk_t* __attribute__((always_inline)) HNEXT(heap_chunk_t *a,
        heaptype type) {
    if (type && a->cookie != cgc___hcookie[type])  cgc___heap_cookie_fail();
    return a->next;
}

/**
 * Check heap cookie if enabled, return next chunk.
 * Terminates on error
 *
 * @param a current heap_chunk_t
 * @param type heap type
 * @return a->next if safe, terminate if not
 */
inline heap_chunk_t* __attribute__((always_inline)) HPREV(heap_chunk_t *a,
        heaptype type) {
    if (type && a->cookie != cgc___hcookie[type])  cgc___heap_cookie_fail();
    return a->prev;
}

/**
 * Validate that heap is walkable in all directions
 * Terminates on error
 *
 */
void cgc_checkheap();
#endif
