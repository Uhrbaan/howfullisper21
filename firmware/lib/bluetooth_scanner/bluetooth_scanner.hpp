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
 * @brief Scans for nearby BLE devices and adds new ones to the knownDevices list.
 *
 * The function will initialize the bluetooth, scan for devices for 10 seconds,
 * and will ignore devices with an rssi lower than a predefined threshold.
 *
 * @returns -1 if the BLE was not initialized properly, else it will return the
 * device count.
 */
int scanForBLEDevices();

#endif  // BLUETOOTH_SCANNER_HPP