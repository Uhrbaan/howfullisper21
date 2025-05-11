#ifndef BLUETOOTH_SCANNER_HPP
#define BLUETOOTH_SCANNER_HPP

#include <Arduino.h>
#include <ArduinoBLE.h>
#include <M5Atom.h>

#include <vector>

/**
 * @brief A list of MAC addresses of known BLE devices detected during the last scan.
 *
 * This vector is cleared before each new scan. Devices are added only if they are new
 * and have an RSSI stronger than the defined threshold (e.g., > -80 dBm).
 */
extern std::vector<String> knownDevices;

/**
 * @brief Initializes the BLE stack.
 *
 * Call this once, typically in your setup() function. It starts the BLE subsystem and
 * blocks the program if BLE initialization fails.
 */
void initializeBLE();

/**
 * @brief Flashes the onboard LED with a specified color.
 *
 * @param color The color to display (RGB hex, e.g., 0xffff00 for yellow).
 * @param times Number of times to flash the LED (default is 1).
 */
void flashLED(uint32_t color, int times = 1);

/**
 * @brief Scans for nearby BLE devices and adds new ones to the knownDevices list.
 *
 * Devices are added only if they are not already known and their RSSI is stronger
 * than -80 dBm (i.e., they are not too far). The LED flashes once per new device,
 * and the device info is printed to the Serial monitor.
 */
void scanForBLEDevices();

#endif  // BLUETOOTH_SCANNER_HPP