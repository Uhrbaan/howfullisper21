// #include <Arduino.h>
// #include <M5Atom.h>
// #include <SPI.h>
// #include <esp_event.h>
// #include <esp_netif.h>
// #include <esp_wifi.h>
#include <esp_log.h>
#include <string.h>
#include <time.h>

#include "connectivity.hpp"
#include "events.hpp"
#include "gpio.hpp"

static const char* TAG = "WIFI EXAMPLE";
static int err = 0;

int sock;

// The setup function enables verbose output, serial output, connects the chip
// to wifi and initializes the TCP socket.
void init() {
    esp_log_level_set("*", ESP_LOG_VERBOSE);  // hithest verbosity level

    // initialization
    err = init_gpio();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not initialize GPIO: %s", esp_err_to_name(err));
        return;
    }

    err = init_timer();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not initialize timer: %s", esp_err_to_name(err));
        return;
    }

    // err = init_wifi();
    // if (err != ESP_OK) {
    //     ESP_LOGE(TAG, "Could not initialize wifi: %s", esp_err_to_name(err));
    //     exit(1);
    // }

    // event handlers
    err = esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to link wifi event handler: %s", esp_err_to_name(err));
        ;
    }

    err = esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, ip_event_handler, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to link ip event handler: %s", esp_err_to_name(err));
        ;
    }

    err =
        esp_event_handler_register(BUTTON_EVENT, BUTTON_PRESSED_EVENT, button_event_handler, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register button event handler: %s", esp_err_to_name(err));
        return;
    }

    err = esp_event_handler_register(TIMER_EVENT, TIMER_ELAPSED_EVENT, timer_event_handler, NULL);

    // init_tcp();
}

// static bool connected = false;
// char response_buff[RESPONSE_BUFFER_SIZE] = {0};
// char payload[RESPONSE_BUFFER_SIZE] = {0};

// static const char* dst_address = "duifvm30";

// void loop() {
//     if (M5.Btn.wasPressed()) {
//         // steps to send data:
//         // 1. create a tcp socket
//         // 2. connect to the tcp socket
//         // 3. send data
//         // 4. recieve data
//         // 5. close socket

//         // 1. create the socket
//         err = create_tcp_socket();
//         if (err != ESP_OK) {
//             ESP_LOGE(TAG, "Could not create a tcp socket, skipping: %s", esp_err_to_name(err));
//             goto close_socket;
//         }

//         // 2. connect to the tcp socket
//         err = connect_tcp_socket();
//         if (err != ESP_OK) {
//             ESP_LOGE(TAG, "Could not connect to the tcp socker, skipping: %s",
//                      esp_err_to_name(err));
//             goto close_socket;
//         }

//         // 3. send data
//         generate_post_request(payload, sizeof(payload), dst_address, 80, "test", 0);
//         if (err != ESP_OK) {
//             ESP_LOGE(TAG, "Could not send payload over tcp, skipping: %s", esp_err_to_name(err));
//             goto close_socket;
//         }

//         // 4. recieve data
//         err = receive_tcp(response_buff);
//         if (err != ESP_OK) {
//             ESP_LOGE(TAG, "Could not recieve the response, skipping: %s", esp_err_to_name(err));
//             goto close_socket;
//         }
//         ESP_LOGI(TAG, "Recieved payload: %s", response_buff);

//     // 5. close tcp socket.
//     close_socket:
//         err = close_tcp_socket();
//         if (err != ESP_OK) {
//             ESP_LOGE(TAG, "could not close the socket: %s", esp_err_to_name(err));
//         }
//     }
// }