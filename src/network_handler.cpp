#include <M5Atom.h>
#include <esp_netif.h>
#include <esp_wifi.h>

static int err = 0;

void wifi_event_handler(void* arg, esp_event_base_t event_base, int event_id, void* event_data) {
    const char* TAG = "WIFI EVENT HANDLER";
    switch (event_id) {
        case WIFI_EVENT_STA_START:
            err = esp_wifi_connect();
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Error in wifi_event_handler: %s\n", esp_err_to_name(err));
            }
            ESP_LOGI(TAG, "Attempting to connect to Wifi...\n");
            break;

        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "Disconected from Wifi. Retrying..\n");
            err = esp_wifi_connect();
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Error in wifi_event_handler: %s\n", esp_err_to_name(err));
            }
            break;

        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "Connected to wifi.");
            break;

        default:
            break;
    }
}