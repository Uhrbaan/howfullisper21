#include <M5Atom.h>
#include <esp_netif.h>
#include <esp_netif_ip_addr.h>
#include <esp_wifi.h>

static int err = 0;

int connected_to_wifi = false;

void wifi_event_handler(void* arg, esp_event_base_t event_base, int event_id,
                        void* event_data) {
    const char* TAG = "WIFI EVENT HANDLER";
    switch (event_id) {
        case WIFI_EVENT_STA_START:
            err = esp_wifi_connect();
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Error in wifi_event_handler: %s\n",
                         esp_err_to_name(err));
            }
            ESP_LOGI(TAG, "Attempting to connect to Wifi...\n");
            M5.dis.drawpix(0, 0x0000ff);
            break;

        case WIFI_EVENT_STA_DISCONNECTED:
            connected_to_wifi = false;
            ESP_LOGI(TAG, "Disconected from Wifi. Retrying..\n");
            err = esp_wifi_connect();
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Error in wifi_event_handler: %s\n",
                         esp_err_to_name(err));
            }
            M5.dis.drawpix(0, 0xff0000);
            break;

        case WIFI_EVENT_STA_CONNECTED:
            connected_to_wifi = true;
            ESP_LOGI(TAG, "Connected to wifi.");
            M5.dis.drawpix(0, 0x00ff00);
            break;

        default:
            break;
    }
    M5.update();
}

void ip_event_handler(void* arg, esp_event_base_t event_base, int event_id,
                      void* event_data) {
    const char* TAG = "IP EVENT HANDLER";
    switch (event_id) {
        case IP_EVENT_STA_GOT_IP: {  // curly brackets needed to create scope
                                     // for variables
            ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
            ESP_LOGI(TAG, "Got IP address:" IPSTR, IP2STR(&event->ip_info.ip));
            break;
        }

        default:
            break;
    }
}
