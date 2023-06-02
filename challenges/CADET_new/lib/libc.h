#ifndef LIBC_H
#define LIBC_H
typedef cgc_size_t size_t;
int transmit_all(int fd, const char *buf, const size_t size);
int receive_delim(int fd, char *buf, const size_t size, char delim);
#endif
