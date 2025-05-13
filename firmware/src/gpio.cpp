#include "gpio.hpp"

#include <driver/gpio.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_timer.h>

static int err = 0;

static void IRAM_ATTR button_isr_handler(void* arg) {
    (void)arg;
    // Post the custom event
    esp_event_post(BUTTON_EVENT, BUTTON_PRESSED_EVENT, NULL, 0,
                   portMAX_DELAY);  // Use portMAX_DELAY or a specific timeout
}

int init_button() {
    const char* TAG = "BUTTON GPIO";
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_NEGEDGE;        // Interrupt on falling edge
    io_conf.mode = GPIO_MODE_INPUT;               // Set as input
    io_conf.pin_bit_mask = (1ULL << BUTTON_PIN);  // Bit mask for the pin
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;  // Enable pull-up resistor
    gpio_config(&io_conf);

    err = gpio_isr_handler_add((gpio_num_t)BUTTON_PIN, button_isr_handler, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not set up interrupt handler: %s", esp_err_to_name(err));
        return err;
    }
}

static esp_timer_handle_t timer_handle;

static void timer_callback(void* arg) {
    (void)arg;
    // Post the timer event
    esp_event_post(TIMER_EVENT, TIMER_ELAPSED_EVENT, NULL, 0, portMAX_DELAY);
}

int init_timer() {
    const char* TAG = "TIMER SETUP";
    // Set up a timer
    esp_timer_create_args_t timer_args = {
        .callback = timer_callback,
        .name = "five_minute_timer",
    };
    err = esp_timer_create(&timer_args, &timer_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not create the timer: %s", esp_err_to_name(err));
        return err;
    }
    err = esp_timer_start_periodic(timer_handle, 50000);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not start timer: %s", esp_err_to_name(err));
        return err;
    }
}
