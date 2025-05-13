/**
 * @file connectivity.cpp
 * @author Léonard Clément (leonard.clement@unifr.ch)
 * @brief This file contains all the functions related to connectivity, including Wi-Fi and TCP
 * socket handling. The documentation of this file is partially AI-generated. It was reviewd for
 * accuracy.
 * @version 0.1
 * @date 2025-05-08
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "connectivity.hpp"

#include <driver/uart.h>
#include <errno.h>
#include <esp_eap_client.h>
#include <esp_err.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_tls.h>
#include <esp_tls.h>  // Include for esp_tls_get_x509_der
#include <esp_wifi.h>
#include <esp_wpa2.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>

#include "secrets.hpp"

#define RESPONSE_BUFFER_SIZE 4096  ///< Adjust as needed for the TCP response buffer size.

static int err = 0;             ///< Global variable to store error codes.
int connected_to_wifi = false;  ///< Global flag indicating Wi-Fi connection status.
static int sock;                ///< Static variable to hold the TCP socket descriptor.
static sockaddr_in dest_addr = {
    0};  ///< Static structure to store the destination IP address and port.

static const int connection_attempts =
    10;  ///< Constant defining the number of connection attempts for TCP.
static bool is_connected =
    false;  ///< Static flag indicating if a TCP connection is currently established.

// temporary
#define HOST_IP "diufvm30"
#define HOST_PORT 80
#define HOST_PORT_STR "80"

/**
 * @brief Initializes the Wi-Fi connection in Station (STA) mode.
 *
 * This function initializes the TCP/IP stack, creates the default event loop and
 * Wi-Fi STA network interface, initializes the Wi-Fi driver, sets the Wi-Fi mode
 * to STA, registers event handlers for Wi-Fi and IP events, configures the Wi-Fi
 * connection details (SSID and password), applies the configuration, and starts
 * the Wi-Fi driver.
 *
 * @return esp_err_t ESP_OK if initialization is successful, otherwise an error code.
 */
int init_wifi() {
    const char* TAG = "WIFI INIT";

    err = esp_netif_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initilize netif: %s", esp_err_to_name(err));
        return err;
    }

    err = esp_event_loop_create_default();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create default envent loop: %s", esp_err_to_name(err));
        return err;
    }

    esp_netif_t* sta_netif = esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    err = esp_wifi_init(&cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize Wi-Fi: %s", esp_err_to_name(err));
        return err;
    }

    err = esp_wifi_set_mode(WIFI_MODE_STA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi mode: %s", esp_err_to_name(err));
        return err;
    }

    wifi_config_t wifi_config = {0};
    strcpy((char*)wifi_config.sta.ssid, ssid);
    wifi_config.sta.pmf_cfg.required = true;

    err = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register wifi configuration: %s", esp_err_to_name(err));
        return err;
    }

    err = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not change wifi storage to RAM: %s", esp_err_to_name(err));
        return err;
    }

    // configure wpa
    err = esp_eap_client_set_identity((uint8_t*)wpa_ent_identity, strlen(wpa_ent_identity));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set wpa identity: %s", esp_err_to_name(err));
        return err;
    }
    err = esp_eap_client_set_username((uint8_t*)wpa_ent_username, strlen(wpa_ent_username));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set wpa username: %s", esp_err_to_name(err));
        return err;
    }
    err = esp_eap_client_set_password((uint8_t*)wpa_ent_password, strlen(wpa_ent_password));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set wpa password: %s", esp_err_to_name(err));
        return err;
    }

    // Set the TTLS phase 2 method
    err = esp_eap_client_set_ttls_phase2_method(ESP_EAP_TTLS_PHASE2_MSCHAPV2);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set the TTLS MschapV2 handshake: %s", esp_err_to_name(err));
        return err;
    }

    err = esp_eap_client_set_ca_cert((const unsigned char*)digicert_global_root_g2_pem,
                                     strlen(digicert_global_root_g2_pem));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "The CA certiicate was not set successfully: %s", esp_err_to_name(err));
        return err;
    }

    err = esp_wifi_sta_enterprise_enable();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "The enterprise wifi station could not be anabled: %s", esp_err_to_name(err));
        return err;
    }
    // TODO: go back to wpa2_ent authentification later.

    // // Set the CA certificate
    // esp_wifi_sta_wpa2_ent_set_ca_cert(
    //     (const unsigned char*)digicert_global_root_g2_pem,
    //     strlen(digicert_global_root_g2_pem) + 1);  // +1 to include null terminator

    err = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi config: %s", esp_err_to_name(err));
        return err;
    }

    err = esp_wifi_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start Wi-Fi: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "Started Wi-Fi in sta mode. Attempting to connect to '%s'", wifi_config.sta.ssid);
    return ESP_OK;
}

/**
 * @brief Creates a TCP socket.
 *
 * This function creates a socket with the AF_INET (IPv4) family and SOCK_STREAM
 * (TCP) type.
 *
 * @return int The socket descriptor if successful, -1 if an error occurred.
 * @retval ESP_OK if the socket could be created.
 * @retval ESP_FAIL if the socket creation failed.
 */
int create_tcp_socket() {
    const char* TAG = "CREATE TCP SOCKET";

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1) {
        ESP_LOGE(TAG, "Failed to create socket: %s", strerror(errno));
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Successfully created socket %d.", sock);
    return ESP_OK;
}

/**
 * @brief Initializes the TCP connection parameters.
 *
 * This function converts the provided HOST_IP address from string format to binary
 * and sets the address family and port number for the destination address.
 *
 * @note HOST_IP and HOST_PORT should be defined elsewhere in the project.
 */
void init_tcp() {
    inet_pton(AF_INET, HOST_IP, &dest_addr.sin_addr);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(HOST_PORT);
}

/**
 * @brief Connects the TCP socket to the server.
 *
 * This function attempts to establish a connection to the server using the IP
 * address and port defined in the static `dest_addr` structure.
 *
 * @return esp_err_t ESP_OK if the connection was successful, otherwise ESP_FAIL.
 */
int connect_tcp_socket() {
    const char* TAG = "CONNECT TCP SOCKET";

    err = connect(sock, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Error connecting to %s:%d. Errno: %s", HOST_IP, HOST_PORT, strerror(errno));
        is_connected = false;
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Succesfully connected to %s:%d", HOST_IP, HOST_PORT);
    is_connected = true;
    return ESP_OK;
}

/**
 * @brief Sends data over the established TCP connection.
 *
 * If a connection is not already established, this function will attempt to
 * connect to the server before sending the data. It retries the connection
 * for a predefined number of attempts.
 *
 * @param buffer A pointer to the buffer containing the data to be sent.
 * @param size The size of the data in bytes.
 * @return esp_err_t ESP_OK if the data was sent successfully, otherwise ESP_FAIL.
 */
int send_tcp(const char* buffer, size_t size) {
    const char* TAG = "SEND TCP";

    if (!is_connected) {
        err = -1;
        for (int i = 0; i < connection_attempts && err != ESP_OK; i++) {
            err = connect_tcp_socket();
            // delay(50);
        }
        if (err != ESP_OK) {
            return ESP_FAIL;
        }
    }

    int len = send(sock, buffer, size, 0);
    if (len == -1) {
        ESP_LOGE(TAG, "Failed to send data.");
        if (errno == EPIPE || errno == ECONNRESET) {
            ESP_LOGE(TAG, "Connection broken.");
            close(sock);
            sock = -1;
            is_connected = false;
        }
        return ESP_FAIL;
    }
    return ESP_OK;
}

/**
 * @brief Receives data over the established TCP connection.
 *
 * This function attempts to receive data from the connected TCP socket. The
 * received data is stored in the provided buffer.
 *
 * @param recv_buff A pointer to the buffer where the received data will be stored.
 * If NULL, the received data will only be logged.
 * @return esp_err_t ESP_OK if data was received successfully (or the connection was closed),
 * otherwise ESP_FAIL if an error occurred during reception.
 */
int receive_tcp(char* recv_buff) {
    const char* TAG = "RECEIVE TCP";
    char response_buffer[RESPONSE_BUFFER_SIZE];
    size_t bytes_received;

    if (sock == -1) {
        ESP_LOGE(TAG, "Socket is not connected.");
        return ESP_FAIL;
    }

    bytes_received = recv(sock, response_buffer, RESPONSE_BUFFER_SIZE - 1, 0);

    if (bytes_received == -1) {
        ESP_LOGE(TAG, "Error receiving data: %d", errno);
        if (errno == EPIPE || errno == ECONNRESET) {
            ESP_LOGE(TAG, "Connection broken during receive: %s", strerror(errno));
            close(sock);
            sock = -1;
            is_connected = false;
        }
        return ESP_FAIL;
    } else if (bytes_received == 0) {
        ESP_LOGI(TAG, "Connection closed by the server.");
        close(sock);
        sock = -1;
        is_connected = false;
        return ESP_OK;
    } else {
        response_buffer[bytes_received] = '\0';
        ESP_LOGI(TAG, "Received response:\n%s", response_buffer);

        if (recv_buff != NULL) {
            strncpy(recv_buff, response_buffer, RESPONSE_BUFFER_SIZE - 1);
            recv_buff[RESPONSE_BUFFER_SIZE - 1] = '\0';  // Ensure null termination
        }

        return ESP_OK;
    }
}

/**
 * @brief Closes the TCP socket.
 *
 * This function closes the currently open TCP socket.
 *
 * @return esp_err_t ESP_OK if the socket was closed successfully, otherwise ESP_FAIL.
 */
int close_tcp_socket() { return (close(sock) == 0) ? ESP_OK : ESP_FAIL; }

/**
 * @brief Creates an HTTP POST request for data collection.
 *
 * Generates a POST request with a JSON payload containing room and count.
 *
 * @param dst Output buffer for the HTTP request.
 * @param size Size of the output buffer.
 * @param ip_addr Target server IP address.
 * @param port Target server port.
 * @param room Room name for the JSON payload.
 * @param count Counter value for the JSON payload.
 *
 * @note Uses a static 256-byte buffer for JSON data.
 */
void generate_post_request(char* dst, size_t size, const char* ip_addr, int port, const char* room,
                           int count) {
    static char json_data[256] = {0};

    snprintf(json_data, 256, "{\n\t\"room\": \"%s\",\n\t\"count\": %d\n}", room, count);

    snprintf(dst, size,
             "POST /collect HTTP/1.1\n"
             "Host: %s:%d\n"
             "Content-Length: %d\n"
             "Content-Type: application/json\n"
             "\n"
             "%s",
             ip_addr, port, strlen(json_data), json_data);
}