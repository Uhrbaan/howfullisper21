#ifndef GPIO_HPP
#define GPIO_HPP

#include <driver/gpio.h>

const int BUTTON_PIN = GPIO_NUM_33;
const int LED_PIN = GPIO_NUM_27;
#define LED_COUNT 1

esp_event_base_t BUTTON_EVENT = "BUTTON_EVENT";  // Or define it as "BUTTON_EVENT"
#define BUTTON_PRESSED_EVENT 0  // id of the button press. Would use an enum if there were multiple

int init_gpio();

esp_event_base_t TIMER_EVENT = "TIMER";  // Event base for timer
#define TIMER_ELAPSED_EVENT 0

int init_timer();

#endif