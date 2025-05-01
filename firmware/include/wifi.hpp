#ifndef INIT_H
#define INIT_H

/**
 * @brief Initializes and connects to wifi.
 *
 * This function initializes the Wifi module of the esp32 chip. It the connects
 * to a predefined wifi network.
 *
 * @return ESP_OK if was initialized and connected successfully, ESP_FAIL
 * otherwise.
 */
int init_wifi();

#endif