#include <M5Atom.h>
#include <ArduinoBLE.h>

void setup()
{
    M5.begin(true, false, true);
    delay(50);
    M5.dis.drawpix(0, 0x00ff00);
    Serial.begin(115200);

    // Bluetooth
    int ok = BLE.begin();
    if (!ok) {
        Serial.println("starting Bluetooth® Low Energy module failed!");
        while (1);
    } else {
        Serial.println("Bluetooth® Low Energy Central scan");
    }

    BLE.scan();
}

uint8_t FSM = 0; // Store the number of key presses.

void loop() {
    if (M5.Btn.wasPressed()) {
        Serial.println("Click!");
        
        switch (FSM) {
            case 0:
                M5.dis.drawpix(0, 0xfff000); // YELLOW
                break;
            case 1:
                M5.dis.drawpix(0, 0xff0000); // RED
                break;
            case 2:
                M5.dis.drawpix(0, 0x0000f0); // BLUE
                break;
            case 3:
                M5.dis.drawpix(0, 0x00ff00); // GREEN
                break;
            default:
                break;
        }

        FSM = ++FSM % 4;

        BLEDevice peripheral = BLE.available();

        if (peripheral) {
            // discovered a peripheral
            Serial.println("Discovered a peripheral");
            Serial.println("-----------------------");
        
            // print address
            Serial.print("Address: ");
            Serial.println(peripheral.address());
        
            // print the local name, if present
            if (peripheral.hasLocalName()) {
              Serial.print("Local Name: ");
              Serial.println(peripheral.localName());
            }
        }
        
        if (peripheral.hasAdvertisedServiceUuid()) {
            Serial.print("Service UUIDs: ");
            for (int i = 0; i < peripheral.advertisedServiceUuidCount(); i++) {
              Serial.print(peripheral.advertisedServiceUuid(i));
              Serial.print(" ");
            }
            Serial.println();
        }

        // print the RSSI
        Serial.print("RSSI: ");
        Serial.println(peripheral.rssi());

        Serial.println();
    }

    delay(50);
    M5.update();
}