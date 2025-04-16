#ifndef NETWORK_HANDLER_H
#define NETWORK_HANDLER_H

#include <esp_wifi.h>

extern int connected_to_wifi;

void wifi_event_handler(void* arg, esp_event_base_t event_base, int event_id,
                        void* event_data);
void ip_event_handler(void* arg, esp_event_base_t event_base, int event_id,
                      void* event_data);

#endif