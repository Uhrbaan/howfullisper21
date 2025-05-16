/**
 * @file connectivity.hpp
 * @author Léonard Clément (leonard.clement@unifr.ch)
 * @brief This file declares connectivity-related functions.
 * @version 0.1
 * @date 2025-05-08
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef CONNECTIVITY_HPP
#define CONNECTIVITY_HPP

#include <esp_wifi.h>

#include <cstddef>

#define RESPONSE_BUFFER_SIZE 4096  ///< Adjust as needed for response buffer.

/**
 * @brief Initializes and connects to Wi-Fi.
 *
 * Initializes the ESP32's Wi-Fi module and connects to a predefined network.
 *
 * @return esp_err_t ESP_OK if successful, ESP_FAIL otherwise.
 */
int init_wifi();

/**
 * @brief Initializes TCP connection parameters and creates a socket.
 *
 * Sets up the necessary information for connecting to the server and creates a TCP socket.
 */
void init_tcp();

/**
 * @brief Creates a TCP socket.
 *
 * @return int Socket descriptor, or -1 on failure.
 * @retval ESP_OK if socket creation is successful.
 * @retval ESP_FAIL if socket creation fails.
 */
int create_tcp_socket();

/**
 * @brief Connects the TCP socket to the server.
 *
 * @return esp_err_t ESP_OK if connection is successful, ESP_FAIL otherwise.
 */
int connect_tcp_socket();

/**
 * @brief Sends data to the server over TCP.
 *
 * Sends the provided buffer to the server. Establishes a connection if one doesn't exist.
 *
 * @param buffer Data to send.
 * @param size Size of the data.
 * @return esp_err_t ESP_OK if data is sent, ESP_FAIL otherwise.
 */
int send_tcp(const char* buffer, size_t size);

/**
 * @brief Receives data from the server over TCP.
 *
 * Reads the server's response into the provided buffer.
 *
 * @param recv_buff Buffer to store the received data.
 * @return esp_err_t Error code.
 */
int receive_tcp(char* recv_buff);

/**
 * @brief Closes the TCP socket.
 *
 * @return esp_err_t ESP_OK if the socket is closed, ESP_FAIL otherwise.
 */
int close_tcp_socket();

/**
 * @brief Indicates if the ESP32 is connected to Wi-Fi.
 *
 */
extern bool connected_to_wifi;

extern bool aquired_ip_addr;

/**
 * @brief Handles Wi-Fi events.
 *
 * Responds to Wi-Fi connection events (start, connect, disconnect) and updates the LED status.
 *
 * @param arg Unused.
 * @param event_base Wi-Fi event base.
 * @param event_id Wi-Fi event ID.
 * @param event_data Unused.
 */
void wifi_event_handler(void* arg, esp_event_base_t event_base, int event_id, void* event_data);

/**
 * @brief Handles IP-related events.
 *
 * Currently only processes the event when an IP address is obtained and logs it.
 *
 * @param arg Unused.
 * @param event_base IP event base.
 * @param event_id IP event ID (only IP_EVENT_STA_GOT_IP handled).
 * @param event_data IP event data.
 */
void ip_event_handler(void* arg, esp_event_base_t event_base, int event_id, void* event_data);

/**
 * @brief Creates an HTTP POST request for data collection.
 *
 * Generates a POST request with a JSON payload containing room and count.
 *
 * @param dst Output buffer for the HTTP request.
 * @param size Size of the output buffer.
 * @param room Room name for the JSON payload.
 * @param count Counter value for the JSON payload.
 *
 * @note Uses a static 256-byte buffer for JSON data.
 */
void generate_post_request(char* dst, size_t size, const char* room, int count);

int shutdown_wifi();

#endif