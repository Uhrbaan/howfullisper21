#include <Arduino.h>
#include <M5Atom.h>
// #include <SPI.h>
// #include <esp_event.h>
// #include <esp_netif.h>
// #include <esp_wifi.h>

#include "connection.hpp"
#include "network_handler.hpp"
#include "wifi.hpp"

static const char* TAG = "WIFI EXAMPLE";
static int err = 0;

int sock;

// The setup function enables verbose output, serial output, connects the chip
// to wifi and initializes the TCP socket.
void setup() {
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

    err = init_tcp();
    delay(5000);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could initialize tcp conneciton: %s",
                 esp_err_to_name(err));
        exit(1);
    }
}

static bool connected = false;
static const char payload[100] = "Hello, World !\n";

// This loop continuesly checks if the button was pressed, and once it is
// pressed, sends a "Hellow, World!\n" message to the server it tries to connect
// to. You can listen to the output of the function with netcat.
void loop() {
    if (M5.Btn.wasPressed()) {
        err = send_tcp(payload, sizeof(payload));
        if (err != ESP_OK) {
            // reset connection if cannot send
            err = init_tcp();
            if (err != ESP_OK) {
                ESP_LOGE(TAG,
                         "Could not re-establish conneciton. Rebooting: %s",
                         esp_err_to_name(err));
                close_tcp_socket();
                exit(1);
            }
        }
    }

    delay(50);
    M5.update();
}