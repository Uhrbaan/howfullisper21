#ifndef CONNECTION_H
#define CONNECTION_H
#include <cstddef>

int init_tcp();
// int create_tcp_socket();
// int connect_tcp_socket();
int send_tcp(const char *buffer, size_t size);
int close_tcp_socket();

#endif