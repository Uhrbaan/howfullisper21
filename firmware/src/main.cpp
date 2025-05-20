#include <Arduino.h>
#include <M5Atom.h>
// #include <SPI.h>
// #include <esp_event.h>
// #include <esp_netif.h>
// #include <esp_wifi.h>
#include <esp_sleep.h>
#include <string.h>

#include "bluetooth_scanner.hpp"
#include "connectivity.hpp"
#include "room.hpp"

void scan(void);

static const char* TAG = "WIFI EXAMPLE";
static int err = 0;

int sock;

static bool connected = false;
char response_buff[RESPONSE_BUFFER_SIZE] = {0};
char payload[RESPONSE_BUFFER_SIZE] = {0};

static room_data room = LEARNING_LAB0;

void setup() {
    srand(time(NULL));
    M5.begin(true, false, true);
    delay(50);
    Serial.begin(115200);

    esp_log_level_set("*", ESP_LOG_VERBOSE);  // hithest verbosity level

    // create a callback when the button is pressed
    // pinMode(BUTTON_PIN, INPUT_PULLUP);
    // attachInterrupt(BUTTON_PIN, callback, FALLING);

    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    switch (wakeup_reason) {
        case ESP_SLEEP_WAKEUP_UNDEFINED:
            ESP_LOGI(TAG, "Wakeup caused by reset or power-on");
            break;
        case ESP_SLEEP_WAKEUP_EXT0:
            ESP_LOGI(TAG, "Wakeup caused by ext0 trigger");
            break;
        case ESP_SLEEP_WAKEUP_EXT1:
            ESP_LOGI(TAG, "Wakeup caused by ext1 trigger");
            break;
        case ESP_SLEEP_WAKEUP_TIMER:
            ESP_LOGI(TAG, "Wakeup caused by timer");
            break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD:
            ESP_LOGI(TAG, "Wakeup caused by touch pad");
            break;
        case ESP_SLEEP_WAKEUP_ULP:
            ESP_LOGI(TAG, "Wakeup caused by ULP program");
            break;
        case ESP_SLEEP_WAKEUP_GPIO:
            ESP_LOGI(TAG, "Wakeup caused by GPIO");
            break;
        case ESP_SLEEP_WAKEUP_UART:
            ESP_LOGI(TAG, "Wakeup caused by UART");
            break;
        default:
            ESP_LOGI(TAG, "Wakeup caused by unknown reason (%d)", wakeup_reason);
            break;
    }

    ESP_LOGI(TAG, "Performing the scan of local area...");

    scan();

    const int deep_sleep_seconds = 1 * 60;
    ESP_LOGI(TAG, "Tasks completed. Entering deep sleep for %d seconds...", deep_sleep_seconds);
    esp_sleep_enable_timer_wakeup(deep_sleep_seconds * 1000000ULL);

    deinit_global_connection();

    esp_deep_sleep_start();
    ESP_LOGE(TAG, "Failed to enter deep sleep!");
}

void scan(void) {
    init_eduroam();

    while (!connected_to_wifi || !aquired_ip_addr) {
        delay(50);
    }

    // begin by scanning for bluetooth:
    room.device_count = scanForBLEDevices();
    if (room.device_count < 0) {
        ESP_LOGE(TAG,
                 "BLE could not be initialized properly. This means something is probably "
                 "wrong with the device.");
        return;
    }

    calculate_room_occupancy(&room);

    // 0. connect to wifi
    // err = init_wifi();
    // if (err != ESP_OK) {
    //     ESP_LOGE(TAG, "Could not initialize wifi: %s", esp_err_to_name(err));
    //     exit(1);
    // }

    init_tcp();

    // 1. create the socket
    err = create_tcp_socket();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not create a tcp socket, skipping: %s", esp_err_to_name(err));
        goto close_socket;
    }

    // 2. connect to the tcp socket
    err = connect_tcp_socket();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not connect to the tcp socket, skipping: %s", esp_err_to_name(err));
        goto close_socket;
    }

    // 3. send data
    static char json_data[200] = {0};
    room_data_to_json(json_data, sizeof(json_data), room);
    generate_post_request(payload, sizeof(payload), json_data);
    err = send_tcp(payload, strlen(payload));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not send payload over tcp, skipping: %s", esp_err_to_name(err));
        goto close_socket;
    }
    ESP_LOGI(TAG, "Successfully sent following payload: %s\n", payload);

    // 4. recieve data
    err = receive_tcp(response_buff);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not recieve the response, skipping: %s", esp_err_to_name(err));
        goto close_socket;
    }
    ESP_LOGI(TAG, "Recieved payload: %s", response_buff);

    delay(100);

// 5. close tcp socket.
close_socket:
    err = close_tcp_socket();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "could not close the socket: %s", esp_err_to_name(err));
    }

    deinit_wifi_connection();
}

void loop() {};