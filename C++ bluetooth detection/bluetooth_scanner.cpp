#include <ArduinoBLE.h>
#include <FS.h>
#include <M5Atom.h>
#include <SPIFFS.h>

#include <algorithm>
#include <vector>

/*
some information about the code. For now:

# it scans for BLE devices every 5 seconds and registers it in the SPIFFS,
  in devices.txt.

# it registers normally only devices with new MAC adresses
  and doesn't tell yet if one of the detected device is gone.

# the RSSI is the signal strength so very useful to know aproximately the
distance: -30 dBm = very close -60 dBm = ~2–5 meters -90 dBm = near limit of
detection

# SPIFFS = SPI Flash File System
    It's a way to save data directly to the internal flash memory of your ESP32
board (no SD card needed). You can write to files like on a computer: Create a
text file Append lines (like MAC + timestamp) Read it later via serial Think of
it like a tiny built-in USB stick.


next things to do:
# we will discuss it but I think the goal would be to scan every x min
  and list all the devices at that moment independently from the last scan
  so it actually tells how many devices there are at that moment.

# determine the range of detection

# test if the adress of a device is always the same (just to be sure)

# implement the transmition of the data to the server

*/

std::vector<String> knownDevices;
unsigned long lastScanTime = 0;
const unsigned long scanInterval = 5000;  // scans every 5 seconds
String serialInput = "";

void setup() {
    M5.begin(true, false, true);  // LED, Serial, I2C (in that order)
    Serial.begin(115200);
    delay(100);

    Serial.println("BLE Scanner");

    // Start SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS failed to start!");
    }

    if (!BLE.begin()) {
        Serial.println("Failed to start BLE!");
        while (1);
    }

    BLE.scan();

    // Show startup message
    Serial.println("Scanning...");
}

// makes the LED flash when a new device is detected
void flashLED(uint32_t color, int times = 1) {
    for (int i = 0; i < times; ++i) {
        M5.dis.drawpix(0, color);  // Flash color
        delay(200);
        M5.dis.drawpix(0, 0x000000);  // Turn off
        delay(200);
    }
}

// saves the detection in devices.txt
void saveDeviceToFile(String mac) {
    File file = SPIFFS.open("/devices.txt", FILE_APPEND);
    if (file) {
        file.printf("%s, time: %lu ms\n", mac.c_str(), millis());
        file.close();
    } else {
        Serial.println("Failed to write to file.");
    }
}

// to see what is registerd in devices.txt
void printSavedDevices() {
    File file = SPIFFS.open("/devices.txt", FILE_READ);
    if (file) {
        Serial.println("=== Saved Devices ===");
        while (file.available()) {
            Serial.write(file.read());
        }
        file.close();
    } else {
        Serial.println("No saved devices or failed to open file.");
    }
}

// allows serial commands to see the list or clear devices.txt in SPIFFS
//(type "list" or "clear" in the serial monitor)
// DOES NOT WORK YET (I wanna sleep)
void handleSerialCommands() {
    while (Serial.available() > 0) {
        char c = Serial.read();

        if (c == '\n' || c == '\r') {
            serialInput.trim();  // Remove extra whitespace

            if (serialInput.equalsIgnoreCase("list")) {
                printSavedDevices();  // Print contents of /devices.txt
            } else if (serialInput.equalsIgnoreCase("clear")) {
                SPIFFS.remove("/devices.txt");
                Serial.println("Device list cleared.");
            } else {
                Serial.println("Unknown command. Try: list, clear");
            }

            serialInput = "";  // Reset input
        } else {
            serialInput += c;
        }
    }
}

void loop() {
    M5.update();
    handleSerialCommands();

    if (millis() - lastScanTime >= scanInterval) {
        lastScanTime = millis();

        BLEDevice device = BLE.available();
        while (device) {
            String address = device.address();

            // If it's a new device
            if (std::find(knownDevices.begin(), knownDevices.end(), address) ==
                knownDevices.end()) {
                knownDevices.push_back(address);

                // Flash LED yellow
                flashLED(0xffff00);

                // Save to file
                saveDeviceToFile(address);

                // Print info
                Serial.println("New device found:");
                int rssi = device.rssi();
                Serial.printf("Device: %s, RSSI: %d dBm\n", address.c_str(),
                              rssi);
                Serial.printf("Devices: %d\n", knownDevices.size());
            }

            device = BLE.available();  // Next device
        }
    }

    delay(100);
}
