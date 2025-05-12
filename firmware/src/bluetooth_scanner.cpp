#include <ArduinoBLE.h>
#include <M5Atom.h>

#include <algorithm>
#include <vector>

std::vector<String> knownDevices;

void initializeBLE() {
    M5.begin(true, false, true);  // LED, Serial, I2C
    Serial.begin(115200);
    delay(100);

    if (!BLE.begin()) {
        Serial.println("Failed to start BLE!");
        while (1);
    }

    Serial.println("Press button to scan for BLE devices.");
}

// Flash the LED with a given color
void flashLED(uint32_t color, int times = 1) {
    for (int i = 0; i < times; ++i) {
        M5.dis.drawpix(0, color);
        delay(200);
        M5.dis.drawpix(0, 0x000000);
        delay(200);
    }
}

// BLE scanning function
int scanForBLEDevices() {
    knownDevices.clear();  // Clean previous scan results
    BLE.scan();            // Start scan

    delay(2000);  // Give it a moment to detect nearby devices

    BLEDevice device = BLE.available();
    while (device) {
        String address = device.address();

        // If it's a new device
        if (std::find(knownDevices.begin(), knownDevices.end(), address) == knownDevices.end()) {
            if (device.rssi() >= -100) {
                knownDevices.push_back(address);

                // Flash LED yellow for each new device
                flashLED(0xffff00);

                // Output to serial
                Serial.printf("New device: %s, RSSI: %d dBm\n", address.c_str(), device.rssi());
            }
        }

        device = BLE.available();
    }

    BLE.stopScan();  // Stop scanning

    return knownDevices.size();
}