#include <Arduino.h>
#include <M5Atom.h>
// #include <SPI.h>
// #include <esp_event.h>
// #include <esp_netif.h>
// #include <esp_wifi.h>
#include <string.h>

#include "connectivity.hpp"

static const char* TAG = "WIFI EXAMPLE";
static int err = 0;

const int BUTTON_PIN = 33;  // Default button pin on M5Stack Atom Lite
                            // (according to some random AI), which is probably
                            // wrong and has to be checked out

int sock;

// The setup function enables verbose output, serial output, connects the chip
// to wifi and initializes the TCP socket.
void setup() {
    srand(time(NULL));
    M5.begin(true, false, true);
    delay(50);
    Serial.begin(115200);

    esp_log_level_set("*", ESP_LOG_VERBOSE);  // hithest verbosity level

    err = init_wifi();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not initialize wifi: %s", esp_err_to_name(err));
        exit(1);
    }

    while (!connected_to_wifi) {
        delay(50);
    }

    init_tcp();

    // create a callback when the button is pressed
    // pinMode(BUTTON_PIN, INPUT_PULLUP);
    // attachInterrupt(BUTTON_PIN, callback, FALLING);
    // TODO: Make a deep-sleep timer that wakes up the esp every 5 minutes,
    // connects it to the internet and does a bluetooth scan.
}

static bool connected = false;
char response_buff[RESPONSE_BUFFER_SIZE] = {0};
char payload[RESPONSE_BUFFER_SIZE] = {0};

static const char* dst_address = "duifvm30";

void loop() {
    if (M5.Btn.wasPressed()) {
        // steps to send data:
        // 1. create a tcp socket
        // 2. connect to the tcp socket
        // 3. send data
        // 4. recieve data
        // 5. close socket

        // 1. create the socket
        err = create_tcp_socket();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Could not create a tcp socket, skipping: %s", esp_err_to_name(err));
            goto close_socket;
        }

        // 2. connect to the tcp socket
        err = connect_tcp_socket();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Could not connect to the tcp socker, skipping: %s",
                     esp_err_to_name(err));
            goto close_socket;
        }

        // 3. send data
        generate_post_request(payload, sizeof(payload), dst_address, 80, "test", 0);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Could not send payload over tcp, skipping: %s", esp_err_to_name(err));
            goto close_socket;
        }

        // 4. recieve data
        err = receive_tcp(response_buff);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Could not recieve the response, skipping: %s", esp_err_to_name(err));
            goto close_socket;
        }
        ESP_LOGI(TAG, "Recieved payload: %s", response_buff);

    // 5. close tcp socket.
    close_socket:
        err = close_tcp_socket();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "could not close the socket: %s", esp_err_to_name(err));
        }
    }

    delay(50);
    M5.update();
}