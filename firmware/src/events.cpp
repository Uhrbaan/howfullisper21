#include <esp_log.h>

#include "connectivity.hpp"
#include "gpio.hpp"

static int err;

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
void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id,
                        void* event_data) {
    const char* TAG = "WIFI EVENT HANDLER";
    switch (event_id) {
        case WIFI_EVENT_STA_START:
            err = esp_wifi_connect();
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Error in wifi_event_handler: %s\n", esp_err_to_name(err));
            }
            ESP_LOGI(TAG, "Attempting to connect to Wifi...\n");
            // M5.dis.drawpix(0, 0x0000ff);  // Blue: Connecting
            break;

        case WIFI_EVENT_STA_DISCONNECTED:
            connected_to_wifi = false;
            ESP_LOGI(TAG, "Disconected from Wifi. Retrying..\n");
            err = esp_wifi_connect();
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Error in wifi_event_handler: %s\n", esp_err_to_name(err));
            }
            // M5.dis.drawpix(0, 0xff0000);  // Red: Disconnected
            break;

        case WIFI_EVENT_STA_CONNECTED:
            connected_to_wifi = true;
            ESP_LOGI(TAG, "Connected to wifi.");
            // M5.dis.drawpix(0, 0x00ff00);  // Green: Connected
            break;

        default:
            break;
    }
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
void ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    const char* TAG = "IP EVENT HANDLER";
    switch (event_id) {
        case IP_EVENT_STA_GOT_IP: {
            ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
            ESP_LOGI(TAG, "Got IP address:" IPSTR, IP2STR(&event->ip_info.ip));
            break;
        }

        default:
            break;
    }
}

void button_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id,
                          void* event_data) {
    const char* TAG = "BUTTON EVENT HANDLER";
    switch (event_id) {
        case BUTTON_PRESSED_EVENT:
            ESP_LOGI(TAG, "Button pressed!");
            //  Add your code here to handle the button press event.
            //  For example, you might want to send data over TCP,
            //  trigger another action, or update the UI.
            break;
        default:
            break;
    }
}

void timer_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id,
                         void* event_data) {
    const char* TAG = "TIMER EVENT HANDLER";
    switch (event_id) {
        case TIMER_ELAPSED_EVENT:
            ESP_LOGI(TAG, "Timer elapsed!");
            break;
        default:
            break;
    }
}