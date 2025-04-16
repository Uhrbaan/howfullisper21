#include <M5Atom.h>
#include <SPI.h>
#include <driver/uart.h>
#include <errno.h>
#include <esp_event.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <sys/socket.h>

#include "network_handler.hpp"

static int err = 0;

int init_wifi() {
    const char *TAG = "WIFI INIT";

    esp_netif_init();  // initializes the underlying TCP/IP stack for networking
    // on the ESP32
    esp_event_loop_create_default();  // creates the default event loop that
                                      // allows different parts of the system to
                                      // communicate via events
    esp_netif_create_default_wifi_sta();  // creates a default network interface
                                          // specifically for the Wi-Fi Station
                                          // mode (chip acts as a client)
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    err = esp_wifi_init(&cfg);  // initializes Wi-Fi
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize Wi-Fi: %s", esp_err_to_name(err));
        return err;
    }

    err = esp_wifi_set_mode(
        WIFI_MODE_STA);  // sets the wifi mode to sta (client not hotspot (ap))
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi mode: %s", esp_err_to_name(err));
        return err;
    }

    // creating some event handlers for wifi
    // TODO: handle errors
    err = esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                     &wifi_event_handler, NULL);
    err = esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID,
                                     &ip_event_handler, NULL);

    // structure containing wifi configuration (made to go on my hotspot)
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = {'u', 'h', 'r', 'b', 'a', 'a', 'n',
                     0},  // has to be written like that because gcc is a b*tch
            .password = {'1', '2', '3', '4', '5', '6', '7', '8',
                         0},  // same here
            .threshold = {.authmode = WIFI_AUTH_WPA_PSK}}};

    err = esp_wifi_set_config(WIFI_IF_STA,
                              &wifi_config);  // applies configuration
    if (err != ESP_OK) {
        ESP_LOGE(TAG, , "Failed to set Wi-Fi config: %s", esp_err_to_name(err));
        return err;
    }

    err = esp_wifi_start();  // start the wifi driver
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start Wi-Fi: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "Started Wi-Fi in sta mode. Attempting to connect to '%s'",
             wifi_config.sta.ssid);
    return ESP_OK;
}