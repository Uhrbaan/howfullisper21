#ifndef EVENTS_HPP
#define EVENTS_HPP

#include <esp_event.h>
#include <inttypes.h>

void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void button_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id,
                          void* event_data);
void timer_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id,
                         void* event_data);

#endif