/**
 * @file secrets.hpp
 * @brief The corresponding file (secrets.cpp) should be added to the src/ folder, and must contain
 * the variables defined in this file. This will configure the wifi connection. To gather
 * information about a specific wifi, you can use `nmcli connection show '<wifi-name>'`
 *
 * @version 0.1
 * @date 2025-05-11
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef SECRETS_HPP
#define SECRETS_HPP
#include <esp_wifi.h>
// extern const char digicert_global_root_g2_pem[];
// extern const char wpa_ent_identity[];
// extern const char wpa_ent_username[];
// extern const char wpa_ent_password[];

/**
 * @brief Weather the connection is of WPA-enterprise type.
 * To find out what kind of security your network has, run `nmcli -g
 * '802-11-wireless-security.key-mgmt' connection show '<wifi-name>'`
 *
 * Currently supported wifis are TODO: `wpa-eap` (set to `true`) and `wpa-psk`
 */
extern const wifi_auth_mode_t authmode;

/**
 * @brief The name of the the wifi you whish to connect to.
 * To find out what the name is, run `nmcli -g 'connection.id' connection show '<wifi-name>'` field.
 */
extern const char ssid[];

/**
 * @brief The password to the wifi network.
 * To find out the password to the network, run `nmcli -g '802-1x.password' -g
 * '802-11-wireless-security.psk' connection show '<wifi-name>' --show-secrets`
 */
extern const char password[];

/**
 * @brief The identity for WPA enterprise netowrks.
 *
 */
extern const char eap_identity[];

/**
 * @brief The username for WPA enterprise networks.
 *
 */
extern const char eap_username[];

extern const char target_ipv4_address[];

extern const char *ca_certificate;

extern const int target_port;

#endif