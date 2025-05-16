#include <ArduinoBLE.h>
#include <M5Atom.h>

#include <algorithm>
#include <vector>

const char TAG[] = "BLE SCANNER";

std::vector<String> knownDevices;

// BLE scanning function
int scanForBLEDevices() {
    if (!BLE.begin()) {
        ESP_LOGE(TAG, "Failed to start BLE!");
        M5.dis.drawpix(0, 0xff0000);
        return -1;
    }
    ESP_LOGI(TAG, "Successfully enabled BLE. Scanning for devices...");
    M5.dis.drawpix(0, 0xffff00);

    knownDevices.clear();  // Clean previous scan results
    BLE.scan();            // Start scan

    delay(10000);  // Give it a moment (10 seconds) to detect nearby devices

    BLEDevice device = BLE.available();
    while (device) {
        String address = device.address();
        // If it's a new device
        if (std::find(knownDevices.begin(), knownDevices.end(), address) == knownDevices.end()) {
            if (device.rssi() >= -100) {
                knownDevices.push_back(address);
                // Output to serial
                ESP_LOGV(TAG, "MAC: %s, RSSI: %d dBm, localName: %s\n", address.c_str(),
                         device.rssi(), device.localName());
            }
        }

        device = BLE.available();
    }

    ESP_LOGI(TAG, "BLE scan finished. Found %d devices.", knownDevices.size());  // Add this
    BLE.stopScan();                                                              // Stop scanning
    BLE.end();
    delay(500);  // wait half a second to ensure the bluetooth has been shut down properly
    M5.dis.drawpix(0, 0);
    return knownDevices.size();
}