#include <Arduino.h>
#include <M5Atom.h>
// #include <SPI.h>
// #include <esp_event.h>
// #include <esp_netif.h>
// #include <esp_wifi.h>

#include "init.hpp"
#include "network_handler.hpp"

static const char* TAG = "WIFI EXAMPLE";
static int err = 0;

void setup() {
    M5.begin(true, false, true);
    delay(50);
    M5.dis.drawpix(0, 0x00ff00);
    Serial.begin(115200);

    esp_log_level_set("*", ESP_LOG_VERBOSE);  // hithest verbosity level

    err = init_wifi();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not initialize wifi: %s", esp_err_to_name(err));
        exit(1);
    }
}

uint8_t FSM = 0;  // Store the number of key presses.

void loop() {
    if (M5.Btn.wasPressed()) {
        Serial.println("Click!");

        switch (FSM) {
            case 0:
                M5.dis.drawpix(0, 0xfff000);  // YELLOW
                break;
            case 1:
                M5.dis.drawpix(0, 0xff0000);  // RED
                break;
            case 2:
                M5.dis.drawpix(0, 0x0000f0);  // BLUE
                break;
            case 3:
                M5.dis.drawpix(0, 0x00ff00);  // GREEN
                break;
            default:
                break;
        }

        FSM = ++FSM % 4;
    }
    delay(50);
    M5.update();
}