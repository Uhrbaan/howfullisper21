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

#include <M5Atom.h>
#include <SPI.h>
#include <arpa/inet.h>
#include <driver/uart.h>
#include <errno.h>
#include <esp_event.h>
#include <esp_netif.h>
#include <esp_tls.h>
#include <esp_wifi.h>
#include <esp_wpa2.h>
#include <sys/socket.h>

#include "secrets.hpp"

#define RESPONSE_BUFFER_SIZE 4096  ///< Adjust as needed for the TCP response buffer size.

bool connected_to_wifi = false;  ///< Global flag indicating Wi-Fi connection status.
bool aquired_ip_addr = false;
bool created_default_loop = false;

static int err = 0;  ///< Global variable to store error codes.
static int sock;     ///< Static variable to hold the TCP socket descriptor.
static sockaddr_in dest_addr = {
    0};  ///< Static structure to store the destination IP address and port.
static esp_netif_t* sta_netif = NULL;

static const int connection_attempts =
    10;  ///< Constant defining the number of connection attempts for TCP.
static bool is_connected =
    false;  ///< Static flag indicating if a TCP connection is currently established.

wifi_config_t wpa_psk_config() {
    wifi_config_t config = {0};
    strcpy((char*)config.sta.ssid, ssid);
    strcpy((char*)config.sta.password, password);
    config.sta.threshold.authmode = WIFI_AUTH_WPA2_WPA3_PSK;
    return config;
}

// right now does not work
wifi_config_t wpa_ent_config() {
    wifi_config_t config = {0};
    strcpy((char*)config.sta.ssid, ssid);
    config.sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
    config.sta.sort_method = WIFI_CONNECT_AP_BY_SIGNAL;
    config.sta.pmf_cfg.capable = true;
    config.sta.pmf_cfg.required = false;

    const char* TAG = "WPA ENT CONFIG";
    // configure wpa
    err = esp_wifi_sta_wpa2_ent_set_identity((uint8_t*)wpa_ent_identity, strlen(wpa_ent_identity));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set wpa identity: %s", esp_err_to_name(err));
        return (wifi_config_t){0};
    }
    err = esp_wifi_sta_wpa2_ent_set_username((uint8_t*)wpa_ent_username, strlen(wpa_ent_username));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set wpa username: %s", esp_err_to_name(err));
        return (wifi_config_t){0};
    }
    err = esp_wifi_sta_wpa2_ent_set_password((uint8_t*)wpa_ent_password, strlen(wpa_ent_password));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set wpa password: %s", esp_err_to_name(err));
        return (wifi_config_t){0};
    }
    // TODO: go back to wpa2_ent authentification later.

    // // Set the CA certificate
    // esp_wifi_sta_wpa2_ent_set_ca_cert(
    //     (const unsigned char*)digicert_global_root_g2_pem,
    //     strlen(digicert_global_root_g2_pem) + 1);  // +1 to include null terminator
    return config;
}

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
    ESP_LOGI(TAG, "Netif initialized successfully.");

    if (created_default_loop == false) {
        err = esp_event_loop_create_default();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create default envent loop: %s", esp_err_to_name(err));
            return err;
        }
        ESP_LOGI(TAG, "Successfully created default event loop.");
        created_default_loop = true;
    }

    if (sta_netif == NULL) {
        sta_netif = esp_netif_create_default_wifi_sta();  // Assign to the global static variable
        if (!sta_netif) {
            ESP_LOGE(TAG, "Failed to create default Wi-Fi STA netif");
            return ESP_FAIL;
        }
    }
    ESP_LOGI(TAG, "Successfully created default netif struct.");

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    err = esp_wifi_init(&cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize Wi-Fi: %s", esp_err_to_name(err));
        return err;
    }
    ESP_LOGI(TAG, "Successfully initialized wifi.");

    err = esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to link wifi event handler: %s", esp_err_to_name(err));
        return err;
    }
    ESP_LOGI(TAG, "Successfully registered wifi event handler.");

    err = esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &ip_event_handler, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to link ip event handler: %s", esp_err_to_name(err));
        return err;
    }
    ESP_LOGI(TAG, "Successfully registered ip event handler.");

    err = esp_wifi_set_mode(WIFI_MODE_STA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi mode: %s", esp_err_to_name(err));
        return err;
    }
    ESP_LOGI(TAG, "Successfully changed wifi to station mode (sta).");

    wifi_config_t wifi_config = {0};
    if (authmode == WIFI_AUTH_WPA2_PSK || authmode == WIFI_AUTH_WPA3_PSK ||
        authmode == WIFI_AUTH_WPA2_WPA3_PSK) {
        wifi_config = wpa_psk_config();
    } else {
        ESP_LOGE(TAG, "The wifi protocol is not supported...");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Successfully configured wifi (mode=%d).", authmode);

    err = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not change wifi storage to RAM: %s", esp_err_to_name(err));
        return err;
    }
    ESP_LOGI(TAG, "Successfully set wifi storage to RAM.");

    err = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi config: %s", esp_err_to_name(err));
        return err;
    }
    ESP_LOGI(TAG, "Successfully set wifi configuration.");

    err = esp_wifi_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start Wi-Fi: %s", esp_err_to_name(err));
        return err;
    }
    ESP_LOGI(TAG, "Successfully started wifi.");

    ESP_LOGI(TAG, "Started Wi-Fi in sta mode. Attempting to connect to '%s'", wifi_config.sta.ssid);
    return ESP_OK;
}

/**
 * @brief Event handler for Wi-Fi events.
 *
 * This function is called by the ESP-IDF event loop when a Wi-Fi related event
 * occurs. It handles events such as station start, disconnection, and connection.
 *
 * @param arg User-defined argument (not used here).
 * @param event_base The base ID of the event (WIFI_EVENT).
 * @param event_id The ID of the specific Wi-Fi event.
 * @param event_data Pointer to the event-specific data.
 */
void wifi_event_handler(void* arg, esp_event_base_t event_base, int event_id, void* event_data) {
    const char* TAG = "WIFI EVENT HANDLER";
    switch (event_id) {
        case WIFI_EVENT_STA_START:
            err = esp_wifi_connect();
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Error in wifi_event_handler: %s\n", esp_err_to_name(err));
            }
            ESP_LOGI(TAG, "Attempting to connect to Wifi...\n");
            M5.dis.drawpix(0, 0x0000ff);  // Blue: Connecting
            break;

        case WIFI_EVENT_STA_DISCONNECTED:
            connected_to_wifi = false;
            ESP_LOGI(TAG, "Disconected from Wifi. Retrying..\n");
            err = esp_wifi_connect();
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Error in wifi_event_handler: %s\n", esp_err_to_name(err));
            }
            M5.dis.drawpix(0, 0xff0000);  // Red: Disconnected
            break;

        case WIFI_EVENT_STA_CONNECTED:
            connected_to_wifi = true;
            ESP_LOGI(TAG, "Connected to wifi.");
            M5.dis.drawpix(0, 0x00ff00);  // Green: Connected
            break;

        default:
            break;
    }
    M5.update();
}

/**
 * @brief Event handler for IP events.
 *
 * This function is called by the ESP-IDF event loop when an IP related event
 * occurs. It handles events such as the acquisition of an IP address.
 *
 * @param arg User-defined argument (not used here).
 * @param event_base The base ID of the event (IP_EVENT).
 * @param event_id The ID of the specific IP event.
 * @param event_data Pointer to the event-specific data.
 */
void ip_event_handler(void* arg, esp_event_base_t event_base, int event_id, void* event_data) {
    const char* TAG = "IP EVENT HANDLER";
    switch (event_id) {
        case IP_EVENT_STA_GOT_IP: {
            ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
            ESP_LOGI(TAG, "Got IP address:" IPSTR, IP2STR(&event->ip_info.ip));
            aquired_ip_addr = true;
            break;
        }

        default:
            break;
    }
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
    inet_pton(AF_INET, target_ipv4_address, &dest_addr.sin_addr);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(target_port);
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
        ESP_LOGE(TAG, "Error connecting to %s:%d. Errno: %s", inet_ntoa(dest_addr.sin_addr),
                 ntohs(dest_addr.sin_port), strerror(errno));
        is_connected = false;
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Succesfully connected to %s:%d", inet_ntoa(dest_addr.sin_addr),
             ntohs(dest_addr.sin_port));
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
            delay(50);
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
 * @param room Room name for the JSON payload.
 * @param count Counter value for the JSON payload.
 *
 * @note Uses a static 256-byte buffer for JSON data.
 */
void generate_post_request(char* dst, size_t size, const char* json) {
    snprintf(dst, size,
             "POST /collect HTTP/1.1\n"
             "Host: %s:%d\n"
             "Content-Length: %d\n"
             "Content-Type: application/json\n"
             "\n"
             "%s",
             inet_ntoa(dest_addr.sin_addr), ntohs(dest_addr.sin_port), strlen(json), json);
}

// AI-generated
int deinit_wifi_connection() {
    esp_err_t err;
    const char* TAG = "WIFI DEINIT";

    // 1. Deregister event handlers (important to avoid issues if you re-init later)
    // You should deregister only the handlers you registered in init_wifi
    err = esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to unregister Wi-Fi event handler: %s", esp_err_to_name(err));
        // Don't return, try to proceed with deinitialization
    }

    err = esp_event_handler_unregister(IP_EVENT, ESP_EVENT_ANY_ID, &ip_event_handler);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to unregister IP event handler: %s", esp_err_to_name(err));
        // Don't return, try to proceed with deinitialization
    }
    ESP_LOGI(TAG, "Event handlers unregistered.");

    // 2. Stop the Wi-Fi driver
    ESP_LOGI(TAG, "Stopping Wi-Fi...");
    err = esp_wifi_stop();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to stop Wi-Fi: %s", esp_err_to_name(err));
        return err;
    }
    ESP_LOGI(TAG, "Wi-Fi stopped.");
    is_connected = false;
    aquired_ip_addr = false;

    // 3. Deinitialize the Wi-Fi driver
    ESP_LOGI(TAG, "Deinitializing Wi-Fi...");
    err = esp_wifi_deinit();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to deinitialize Wi-Fi: %s", esp_err_to_name(err));
        return err;
    }
    ESP_LOGI(TAG, "Wi-Fi deinitialized.");

    // Reset your connection flags
    connected_to_wifi = false;
    aquired_ip_addr = false;

    // turn the LED off.
    M5.dis.drawpix(0, 0x000000);

    return ESP_OK;
}

void deinit_global_connection() {
    const char TAG[] = "GLOBAL CONNECTION DEINIT";

    // 4. Delete the default Wi-Fi STA network interface
    // This assumes you used `esp_netif_create_default_wifi_sta()`
    ESP_LOGI(TAG, "Destroying Wi-Fi STA netif...");
    esp_netif_t* default_netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    if (default_netif) {
        esp_netif_destroy(default_netif);
        ESP_LOGI(TAG, "Wi-Fi STA netif destroyed.");
    } else {
        ESP_LOGW(TAG, "Default Wi-Fi STA netif not found, might have been destroyed already.");
    }

    // 5. Deinitialize the default event loop (if it's no longer needed for anything else)
    // Be careful with this if other components rely on the default event loop.
    // If you only use it for Wi-Fi, it's generally safe to delete it.
    ESP_LOGI(TAG, "Destroying default event loop...");
    err = esp_event_loop_delete_default();
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to delete default event loop: %s", esp_err_to_name(err));
        // It's possible other components are still using it, so a warning might be sufficient.
    } else {
        ESP_LOGI(TAG, "Default event loop destroyed.");
    }
}