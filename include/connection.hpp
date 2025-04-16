#ifndef CONNECTION_H
#define CONNECTION_H
#include <cstddef>

/**
 * @brief Initializes the TCP connection to the server.
 *
 * This function initializes the (predefined) data necessary to establish a
 * connection with the server. It also creates a TCP socket for sending the data
 * through.
 *
 * @return ESP_OK if the socket was created successfully, ESP_FAIL otherwise.
 */
int init_tcp();

/**
 * @brief Sends payload `buffer` of size `size` to the server.
 *
 * This function sends the data in `buffer` of size `size` to the server. If no
 * TCP connection has been established with the server, one will be created.
 *
 * @param buffer Data to send to the server.
 * @param size Size of the data to send to the server.
 * @return ESP_OK if the connection is established and the data was sent,
 * ESP_FAIL otherwise.
 */
int send_tcp(const char *buffer, size_t size);
int close_tcp_socket();

#endif