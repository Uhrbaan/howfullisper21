#ifndef NETWORK_HANDLER_H
#define NETWORK_HANDLER_H

#include <esp_wifi.h>

/**
 * @brief Boolean that tracks weather the esp32 is connected to Wifi.
 *
 */
extern int connected_to_wifi;

/**
 * @brief Event handler for the esp32 Wifi events.
 *
 * This function handles Wifi events (WIFI_EVENT). On WIFI_EVENT_STA_START
 * it tries to connect to the wifi and changes the LED to BLUE. On
 * WIFI_EVENT_STA_DISCONNECTED, it re-attempts to connect to the wifi and
 * changes the LED to RED. On WIFI_EVENT_STA_CONNECTED it changes the LED to
 * GREEN.
 *
 * @param arg unused
 * @param event_base unused
 * @param event_id ID of the event (WIFI_EVENT_STA_START,
 * WIFI_EVENT_STA_DISCONNECTED, WIFI_EVENT_STA_CONNECTED are handled)
 * @param event_data unused
 */
void wifi_event_handler(void* arg, esp_event_base_t event_base, int event_id,
                        void* event_data);

/**
 * @brief Event handler for the esp32 IP events
 *
 * This function handles IP events (IP_EVENT). For now, it only handles when the
 * esp32 got its IP address (IP_EVENT_STA_GOT_IP), and prints it as info.
 *
 * @param arg unused
 * @param event_base unused
 * @param event_id only IP_EVENT_STA_GOT_IP is handled
 * @param event_data converted to ip_event_got_ip_t in IP_EVENT_STA_GOT_IP
 */
void ip_event_handler(void* arg, esp_event_base_t event_base, int event_id,
                      void* event_data);

#endif