#include <ArduinoBLE.h>
#include <M5Atom.h>

#include <algorithm>
#include <vector>

const char TAG[] = "BLE SCANNER";

std::vector<String> knownDevices;

static const char *appearance_list[] = {
    "Unknown",
    "Generic Phone",
    "Generic Computer",
    "Generic Watch",
    "Watch (Sports Watch)",
    "Generic Clock",
    "Generic Display",
    "Generic Remote Control",
    "Generic Eye Glasses",
    "Generic Tag",
    "Generic Keyring",
    "Generic Media Player",
    "Generic Barcode Scanner",
    "Generic Thermometer",
    "Thermometer (Ear)",
    "Generic Heart Rate Sensor",
    "Heart Rate Sensor (Heart Rate Belt)",
    "Generic Blood Pressure",
    "Blood Pressure (Arm)",
    "Blood Pressure (Wrist)",
    "Generic HID (Human Interface Device)",
    "HID (Keyboard)",
    "HID (Mouse)",
    "HID (Joystick)",
    "HID (Gamepad)",
    "HID (Digitizer Subtype)",
    "HID (Card Reader)",
    "HID (Digital Pen)",
    "HID (Barcode)",
    "Generic Glucose Meter",
    "Generic Running/Walking Sensor",
    "Running/Walking Sensor (In Shoe)",
    "Running/Walking Sensor (On Shoe)",
    "Running/Walking Sensor (On Hip)",
    "Generic Cycling",
    "Cycling (Cycling Computer)",
    "Cycling (Speed Sensor)",
    "Cycling (Cadence Sensor)",
    "Cycling (Power Sensor)",
    "Cycling (Speed & Cadence Sensor)",
    "Unknown/Unlisted Appearance",
};

const char *getBleAppearanceString(uint16_t appearance) {
    switch (appearance) {
        case 0:
            return appearance_list[0];
        case 64:
            return appearance_list[1];
        case 128:
            return appearance_list[2];
        case 192:
            return appearance_list[3];
        case 193:
            return appearance_list[4];
        case 256:
            return appearance_list[5];
        case 320:
            return appearance_list[6];
        case 384:
            return appearance_list[7];
        case 448:
            return appearance_list[8];
        case 512:
            return appearance_list[9];
        case 576:
            return appearance_list[10];
        case 640:
            return appearance_list[11];
        case 704:
            return appearance_list[12];
        case 768:
            return appearance_list[13];
        case 769:
            return appearance_list[14];
        case 832:
            return appearance_list[15];
        case 833:
            return appearance_list[16];
        case 896:
            return appearance_list[17];
        case 897:
            return appearance_list[18];
        case 898:
            return appearance_list[19];
        case 960:
            return appearance_list[20];
        case 961:
            return appearance_list[21];
        case 962:
            return appearance_list[22];
        case 963:
            return appearance_list[23];
        case 964:
            return appearance_list[24];
        case 965:
            return appearance_list[25];
        case 966:
            return appearance_list[26];
        case 967:
            return appearance_list[27];
        case 968:
            return appearance_list[28];
        case 1024:
            return appearance_list[29];
        case 1088:
            return appearance_list[30];
        case 1089:
            return appearance_list[31];
        case 1090:
            return appearance_list[32];
        case 1091:
            return appearance_list[33];
        case 1152:
            return appearance_list[34];
        case 1153:
            return appearance_list[35];
        case 1154:
            return appearance_list[36];
        case 1155:
            return appearance_list[37];
        case 1156:
            return appearance_list[38];
        case 1157:
            return appearance_list[39];
        default:
            return appearance_list[40];
    }
}

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
        const char *device_type = getBleAppearanceString(device.appearance());
        // If it's a new device
        if (std::find(knownDevices.begin(), knownDevices.end(), address) == knownDevices.end()) {
            if (device.rssi() >= -100) {
                knownDevices.push_back(address);
                // Output to serial
                ESP_LOGV(TAG, "MAC: %s, RSSI: %d dBm, localName: %s\n, type: %s", address.c_str(),
                         device.rssi(), device.localName(), device_type);
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