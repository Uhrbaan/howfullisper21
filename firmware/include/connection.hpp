#ifndef CONNECTION_H
#define CONNECTION_H
#include <cstddef>

#define RESPONSE_BUFFER_SIZE 4096  // Adjust as needed

#define HOST_IP "192.168.188.176"
#define HOST_PORT 5000
#define HOST_PORT_STR "5000"

/**
 * @brief Initializes the TCP connection to the server.
 *
 * This function initializes the (predefined) data necessary to establish a
 * connection with the server. It also creates a TCP socket for sending the data
 * through.
 */
void init_tcp();

int create_tcp_socket();

int connect_tcp_socket();

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

/**
 * @brief Takes the response of a previously made call with `send_tcp`.
 *
 * @param recv_buff Buffer into which the response will be loaded.
 * @return int Error code.
 */
int receive_tcp(char *recv_buff);

int close_tcp_socket();

#endif